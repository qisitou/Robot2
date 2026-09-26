Add-Type @"
using System;
using System.Runtime.InteropServices;
public static class DapProbe {
    [DllImport("D:/keil5/ARM/BIN/CMSIS_DAP.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern int CMSIS_DAP_GetNumberOfDevices();
    [DllImport("D:/keil5/ARM/BIN/CMSIS_DAP.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern int CMSIS_DAP_DetectNumberOfDevices();
    [DllImport("D:/keil5/ARM/BIN/CMSIS_DAP.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern int DAP_GetNumberOfDAPs();
    [DllImport("D:/keil5/ARM/BIN/CMSIS_DAP.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern int DAP_GetInterfaceVersion();
    [DllImport("D:/keil5/ARM/BIN/CMSIS_DAP.dll", CallingConvention = CallingConvention.Cdecl, EntryPoint = "CMSIS_DAP_ConfigureInterface")]
    public static extern int ConfigureInterface0();
    [DllImport("D:/keil5/ARM/BIN/CMSIS_DAP.dll", CallingConvention = CallingConvention.Cdecl, EntryPoint = "CMSIS_DAP_ConfigureDebugger")]
    public static extern int ConfigureDebugger0();
    [DllImport("D:/keil5/ARM/BIN/CMSIS_DAP.dll", CallingConvention = CallingConvention.Cdecl, EntryPoint = "CMSIS_DAP_Connect")]
    public static extern int Connect0();
    [DllImport("D:/keil5/ARM/BIN/CMSIS_DAP.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern int RDDI_Open([MarshalAs(UnmanagedType.LPStr)] string name);
    [DllImport("D:/keil5/ARM/BIN/CMSIS_DAP.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern int RDDI_Close();
    [DllImport("D:/keil5/ARM/BIN/CMSIS_DAP.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern int DAP_Configure(int a, [MarshalAs(UnmanagedType.LPStr)] string config);
    [DllImport("D:/keil5/ARM/BIN/CMSIS_DAP.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern int DAP_GetNumberOfDAPs(int a, out int count);
}
"@

Write-Output ("devices=" + [DapProbe]::CMSIS_DAP_GetNumberOfDevices())
Write-Output ("detect=" + [DapProbe]::CMSIS_DAP_DetectNumberOfDevices())
Write-Output ("daps=" + [DapProbe]::DAP_GetNumberOfDAPs())
Write-Output ("version=" + [DapProbe]::DAP_GetInterfaceVersion())
Write-Output ("configure0=" + [DapProbe]::ConfigureInterface0())
Write-Output ("debugger0=" + [DapProbe]::ConfigureDebugger0())
Write-Output ("connect0=" + [DapProbe]::Connect0())
Write-Output ("open=" + [DapProbe]::RDDI_Open("CMSIS-DAP"))
$count = 0
Write-Output ("num2=" + [DapProbe]::DAP_GetNumberOfDAPs(0, [ref]$count) + ",count=" + $count)
Write-Output ("cfg=" + [DapProbe]::DAP_Configure(0, "Port=SW;SWJ=SW;Clock=1000000;"))
Write-Output ("close=" + [DapProbe]::RDDI_Close())
