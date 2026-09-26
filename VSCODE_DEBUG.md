# 在 VS Code 中编译、下载和调试

本工程使用 Keil 原生的 `UVSC64.dll` 和 CMSIS-AGDI 驱动控制当前 Horco
CMSIS-DAP/DAPLink。VS Code 负责界面和 DAP 协议，后台隐藏启动 `UV4.exe`。

- `F5`：编译、下载并进入调试
- 任务 `DAPLink: flash (Keil UVSC)`：只编译和下载
- `Ctrl+Shift+B`：只编译

如果当前打开的是外层多根工作区 `D:\Game\Robot2`，请使用外层
`.vscode\launch.json`；它会自动指向内层 `Robot2` 工程。如果直接打开
`D:\Game\Robot2\Robot2`，则使用内层 `.vscode\launch.json`。

同一时间只能有一个程序占用 DAPLink。启动 VS Code 调试前不要让另一个 Keil
Debug、OpenOCD 或 pyOCD 会话连接探针。
