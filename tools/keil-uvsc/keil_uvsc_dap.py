#!/usr/bin/env python3
"""Small VS Code DAP adapter backed by Keil's native UVSC interface."""

from __future__ import annotations

import json
import os
import re
import socket
import subprocess
import sys
import threading
import time
from pathlib import Path

from keil_uvsc import Uvsc, UvscError


class DapAdapter:
    def __init__(self):
        self.seq = 1
        self.lock = threading.Lock()
        self.uv = None
        self.uv4 = None
        self.uv4_process = None
        self.project = None
        self.executable = None
        self.target = "hao"
        self.port = 4328
        self.running = False
        self.shutdown = False
        self.last_stopped = False
        self.breakpoints = {}
        self.thread = None

    def send(self, message):
        raw = json.dumps(message, ensure_ascii=False, separators=(",", ":")).encode("utf-8")
        sys.stdout.buffer.write(f"Content-Length: {len(raw)}\r\n\r\n".encode("ascii") + raw)
        sys.stdout.buffer.flush()

    def response(self, request, body=None, success=True, message=None):
        result = {"type": "response", "seq": self.seq, "request_seq": request.get("seq", 0),
                  "command": request.get("command", ""), "success": success}
        self.seq += 1
        if body is not None:
            result["body"] = body
        if message:
            result["message"] = message
        self.send(result)

    def event(self, name, body=None):
        message = {"type": "event", "seq": self.seq, "event": name}
        self.seq += 1
        if body is not None:
            message["body"] = body
        self.send(message)

    def wait_port(self):
        deadline = time.time() + 30
        while time.time() < deadline:
            if self.uv4_process and self.uv4_process.poll() is not None:
                raise RuntimeError(f"UV4 exited with code {self.uv4_process.returncode}")
            with socket.socket() as sock:
                if sock.connect_ex(("127.0.0.1", self.port)) == 0:
                    return
            time.sleep(0.15)
        raise TimeoutError(f"UVSOCK port {self.port} did not open")

    def start_uv4(self):
        self.wait_port_if_existing()
        startup = None
        if os.name == "nt":
            startup = subprocess.STARTUPINFO()
            startup.dwFlags |= subprocess.STARTF_USESHOWWINDOW
            startup.wShowWindow = 0
        self.uv4_process = subprocess.Popen(
            [str(self.uv4 / "UV4.exe"), f"-s{self.port}", str(self.project), "-t", self.target],
            cwd=str(self.project.parent), startupinfo=startup,
        )
        self.wait_port()

    def wait_port_if_existing(self):
        with socket.socket() as sock:
            if sock.connect_ex(("127.0.0.1", self.port)) == 0:
                return

    def connect_uvsc(self):
        last = None
        for _ in range(40):
            try:
                self.uv = Uvsc(self.port, self.uv4)
                self.uv.__enter__()
                return
            except Exception as exc:
                last = exc
                time.sleep(0.25)
        raise last or RuntimeError("cannot connect UVSC")

    def pc(self):
        for reg in self.uv.registers():
            if reg.get("is_pc") or reg.get("name", "").upper() in {"PC", "R15"}:
                try:
                    return int(str(reg["value"]).strip(), 0)
                except ValueError:
                    pass
        return 0

    def address_info(self, address):
        try:
            info = self.uv._address_info(address)
            if info.get("file"):
                return info
        except Exception:
            pass
        return {}

    def gdb_line_address(self, source, line):
        gdb = os.environ.get("ROBOT2_GDB", r"C:\Program Files (x86)\Arm GNU Toolchain arm-none-eabi\14.2 rel1\bin\arm-none-eabi-gdb.exe")
        if not Path(gdb).is_file() or not self.executable:
            return None
        spec = f"{source}:{line}"
        proc = subprocess.run([gdb, "--batch", str(self.executable), "-ex", f"info line {spec}"],
                              capture_output=True, text=True, errors="replace", timeout=8)
        match = re.search(r"starts at address 0x([0-9a-fA-F]+)", proc.stdout)
        return int(match.group(1), 16) if match else None

    def stop_info(self):
        address = self.pc()
        info = self.address_info(address)
        source = info.get("file", "")
        if source and not Path(source).is_absolute():
            source = str((self.project.parent.parent / source).resolve())
        frame = {"id": 1, "name": info.get("function", "main"), "instructionPointerReference": f"0x{address:X}",
                 "line": int(info.get("line", 1) or 1), "column": 1}
        if source:
            frame["source"] = {"name": Path(source).name, "path": source}
        return frame

    def poll(self):
        while not self.shutdown:
            if self.uv is None:
                time.sleep(0.2)
                continue
            try:
                regs = self.uv.registers()
                stopped = True
            except Exception:
                stopped = False
            if stopped != self.last_stopped:
                self.last_stopped = stopped
                if stopped:
                    self.running = False
                    self.event("stopped", {"reason": "breakpoint", "threadId": 1, "allThreadsStopped": True})
                else:
                    self.running = True
                    self.event("continued", {"threadId": 1, "allThreadsContinued": True})
            time.sleep(0.25)

    def launch(self, request):
        args = request.get("arguments", {})
        self.project = Path(args["project"]).resolve()
        self.executable = Path(args.get("executable", "")).resolve()
        self.target = args.get("target", "hao")
        self.port = int(args.get("port", 4328))
        self.uv4 = Path(args.get("uv4Dir", r"D:\keil5\UV4")).resolve()
        self.start_uv4()
        self.connect_uvsc()
        self.uv.control("enter")
        try:
            self.uv.command("BS main")
        except Exception:
            pass
        deadline = time.time() + 12
        while time.time() < deadline:
            try:
                self.uv.registers()
                break
            except Exception:
                time.sleep(0.2)
        self.last_stopped = True
        self.response(request)
        self.event("process", {"name": "UV4.exe", "systemProcessId": self.uv4_process.pid if self.uv4_process else 0})
        self.event("stopped", {"reason": "entry", "threadId": 1, "allThreadsStopped": True})
        self.thread = threading.Thread(target=self.poll, daemon=True)
        self.thread.start()

    def handle(self, request):
        command = request.get("command")
        if command == "initialize":
            self.response(request, {"supportsConfigurationDoneRequest": True, "supportsEvaluateForHovers": True,
                                    "supportsSetVariable": False, "supportsReadMemoryRequest": True,
                                    "supportsStepBack": True})
            self.event("initialized")
        elif command == "launch":
            try:
                self.launch(request)
            except Exception as exc:
                self.response(request, success=False, message=str(exc))
        elif command == "configurationDone":
            self.response(request)
        elif command == "threads":
            self.response(request, {"threads": [{"id": 1, "name": "STM32F407"}]})
        elif command == "setBreakpoints":
            source = request.get("arguments", {}).get("source", {}).get("path", "")
            result = []
            for bp in request.get("arguments", {}).get("breakpoints", []):
                line = int(bp.get("line", 0))
                address = self.gdb_line_address(source, line)
                verified = False
                if address is not None:
                    try:
                        self.uv.command(f"BS 0x{address:X}")
                        verified = True
                    except Exception:
                        pass
                result.append({"id": address or line, "verified": verified, "line": line,
                               "source": {"path": source} if source else None,
                               "instructionReference": f"0x{address:X}" if address else None})
            self.response(request, {"breakpoints": result})
        elif command == "stackTrace":
            frames = []
            try:
                for index, item in enumerate(self.uv.stack()):
                    source = item.get("file", "")
                    if source and not Path(source).is_absolute():
                        source = str((self.project.parent.parent / source).resolve())
                    frame = {"id": index + 1, "name": item.get("function", "?"),
                             "instructionPointerReference": item.get("address", "0x0"),
                             "line": int(item.get("line", 1) or 1), "column": 1}
                    if source:
                        frame["source"] = {"name": Path(source).name, "path": source}
                    frames.append(frame)
            except Exception:
                frames = [self.stop_info()]
            self.response(request, {"stackFrames": frames, "totalFrames": len(frames)})
        elif command == "scopes":
            self.response(request, {"scopes": [{"name": "Registers", "variablesReference": 100, "expensive": False}]})
        elif command == "variables":
            variables = []
            if request.get("arguments", {}).get("variablesReference") == 100:
                try:
                    variables = [{"name": r["name"], "value": r["value"], "type": r.get("group", "register"),
                                  "variablesReference": 0} for r in self.uv.registers()]
                except Exception:
                    pass
            self.response(request, {"variables": variables})
        elif command == "evaluate":
            expression = request.get("arguments", {}).get("expression", "")
            try:
                output = self.uv.command(f"EVAL {expression}").get("output", "")
                self.response(request, {"result": output, "variablesReference": 0})
            except Exception as exc:
                self.response(request, success=False, message=str(exc))
        elif command in {"continue", "pause", "next", "stepIn", "stepOut", "stepBack"}:
            operation = {"continue": "run", "pause": "halt", "next": "step", "stepIn": "step-into",
                         "stepOut": "step-out", "stepBack": "step"}[command]
            try:
                self.uv.control(operation)
                self.response(request, {"allThreadsContinued": command == "continue"} if command == "continue" else {})
            except Exception as exc:
                self.response(request, success=False, message=str(exc))
        elif command == "readMemory":
            args = request.get("arguments", {})
            try:
                address = int(str(args.get("memoryReference", "0")), 0)
                count = int(args.get("count", 1))
                data = bytes.fromhex(self.uv.memory(address, count)["hex"].replace(" ", ""))
                self.response(request, {"address": hex(address), "data": data.hex()})
            except Exception as exc:
                self.response(request, success=False, message=str(exc))
        elif command in {"disconnect", "terminate"}:
            self.response(request)
            self.close()
        else:
            self.response(request, success=True)

    def close(self):
        if self.shutdown:
            return
        self.shutdown = True
        try:
            if self.uv:
                try:
                    self.uv.control("exit")
                except Exception:
                    pass
                self.uv.__exit__(None, None, None)
        finally:
            if self.uv4_process and self.uv4_process.poll() is None:
                self.uv4_process.terminate()

    def run(self):
        while not self.shutdown:
            header = sys.stdin.buffer.readline()
            if not header:
                break
            if not header.lower().startswith(b"content-length:"):
                continue
            length = int(header.split(b":", 1)[1].strip())
            sys.stdin.buffer.readline()
            payload = sys.stdin.buffer.read(length)
            if not payload:
                break
            try:
                self.handle(json.loads(payload.decode("utf-8")))
            except Exception as exc:
                sys.stderr.write(f"DAP error: {exc}\n")
                sys.stderr.flush()
        self.close()


if __name__ == "__main__":
    try:
        DapAdapter().run()
    except Exception as exc:
        print(f"keil_uvsc_dap failed: {exc}", file=sys.stderr)
        sys.exit(1)
