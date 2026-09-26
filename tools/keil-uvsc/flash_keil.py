#!/usr/bin/env python3
from __future__ import annotations

import os
import socket
import subprocess
import sys
import time
import ctypes as C
from pathlib import Path

from keil_uvsc import Uvsc


def main():
    project = Path(sys.argv[1]).resolve()
    uv4_dir = Path(sys.argv[2]).resolve()
    target = sys.argv[3] if len(sys.argv) > 3 else "hao"
    port = int(sys.argv[4]) if len(sys.argv) > 4 else 4329
    startup = None
    if os.name == "nt":
        startup = subprocess.STARTUPINFO()
        startup.dwFlags |= subprocess.STARTF_USESHOWWINDOW
        startup.wShowWindow = 0
    process = subprocess.Popen([str(uv4_dir / "UV4.exe"), f"-s{port}", str(project), "-t", target],
                               cwd=str(project.parent), startupinfo=startup)
    try:
        deadline = time.time() + 30
        while time.time() < deadline:
            with socket.socket() as sock:
                if sock.connect_ex(("127.0.0.1", port)) == 0:
                    break
            if process.poll() is not None:
                raise RuntimeError(f"UV4 exited with code {process.returncode}")
            time.sleep(0.2)
        else:
            raise TimeoutError(f"UVSOCK port {port} did not open")
        uv = Uvsc(port, uv4_dir)
        with uv:
            uv.dll.UVSC_PRJ_FLASH_DOWNLOAD.argtypes = [C.c_int32]
            uv.dll.UVSC_PRJ_FLASH_DOWNLOAD.restype = int
            status = uv.dll.UVSC_PRJ_FLASH_DOWNLOAD(uv.handle.value)
            if status != 0:
                raise RuntimeError(uv.last_error())
        print("Flash Download finished")
    finally:
        if process.poll() is None:
            process.terminate()


if __name__ == "__main__":
    main()
