# 上位机监控项目交接说明

## 当前项目

- 项目源码：`C:\Users\DELL\Documents\遥控器的杂碎事情\CanVariableMonitor`
- 客户同步目录：`F:\工作\AI模型\s上位机\监控上位机\上位机\上位机监控_V1.2_20260612_120554`
- 当前上位机版本：`V1.19`
- 最新发布包：`C:\Users\DELL\Documents\遥控器的杂碎事情\CanVariableMonitor\release\上位机监控_V1.19_20260612_181055.zip`
- 主程序：`上位机监控.exe`

## 这份快照包含

- `workspace.zip`：`CanVariableMonitor` 当前源码和 release 文件。
- `transcript.md`：本轮开发聊天记录。
- `raw-session.jsonl`：脱敏后的原始会话记录，`encrypted_content` 已替换为占位符。
- `support-logs/diagnostic.log`：当前上位机诊断日志。
- `support-logs/connection_status.txt`：最近连接/下载状态。

## 关键现状

- 软件目标是 Keil/CAN 在线变量监控和现场调试，上位机自动读取工作目录内的 map/axf/源码。
- CAN 工具自动识别，不给客户选择下载器：当前兼容 PEAK、SYS、广成 GC。
- 发布采用 `win-x86`，目的是兼容更多客户电脑和厂商 DLL。
- `PublishUnified.ps1` 会发布到 `dist\上位机监控`，并自动同步覆盖客户目录。
- 当前版本 `V1.19` 修复了关闭窗口后进程残留的问题：主窗口关闭后会强制结束进程，释放 PEAK/SYS/GC 句柄。
- `0x16` 是控制器 boot/下载状态发回来的帧，不是上位机发送。模块断电后 0x16 消失，已验证这一判断。
- `RX 0x16 00` 日志已限流，避免 boot 状态下刷爆日志和拖慢界面。

## 最近重点修改

- `Program.cs`
  - `Application.Run(new MainForm())` 外层增加 `finally { Environment.Exit(0); }`，保证窗口关闭后进程彻底退出。
- `MainForm.cs`
  - 版本升级到 `V1.19`。
  - 下载流程支持复用已经打开的 CAN 适配器，减少 PEAK 关闭后立即重开失败的概率。
  - 关闭窗体时取消下载任务、停止轮询、释放适配器。
- `CanAdapters.cs`
  - PEAK 适配器增加 `CAN_Uninitialize(PCAN_NONEBUS)` 清理。
  - 对 PEAK 初始化状态做了额外清理重试。
- `CanFirmwareDownloader.cs`
  - 下载 ACK 等待支持取消。
  - 重复 RX 诊断帧限流。
- `PublishUnified.ps1`
  - 发布后自动同步到客户固定目录。
  - 中文路径用 `[char]` 拼接，避免 PowerShell 脚本编码导致同步到乱码路径。

## 编译和发布

```powershell
cd "C:\Users\DELL\Documents\遥控器的杂碎事情\CanVariableMonitor"
dotnet build .\CanVariableMonitor.csproj
powershell -ExecutionPolicy Bypass -File .\PublishUnified.ps1
```

发布完成后检查：

```powershell
Get-ChildItem ".\release" -Filter "上位机监控_V*.zip" | Sort-Object LastWriteTime -Descending | Select-Object -First 3
Get-ChildItem "F:\工作\AI模型\s上位机\监控上位机\上位机\上位机监控_V1.2_20260612_120554"
```

## 现场调试注意

- 如果 PEAK 下载失败，先看 `%APPDATA%\CanVariableMonitor\diagnostic.log`。
- 如果日志显示 `PEAK PCAN-USB open failed: 0x51=0x4000000`，通常是 PEAK 通道仍被进程占用。先确认任务管理器没有 `上位机监控.exe` 残留；`V1.19` 已针对这个做了硬退出。
- 如果下载时一直收到 `RX 0x016 00`，说明控制器/模块还在 boot 或下载应答状态。它不是上位机发送帧。
- 如果 `未收到 boot 握手`，要检查控制器是否真的被重启进 boot、CAN 波特率/接线是否正常、监控固件是否已加载并下载到控制器。
- 如果点下载前正在监控变量，下载流程会先停止监控，再进入下载。

## 另一台电脑恢复

1. 克隆或拉取 `https://github.com/hudonghua/codex-personal-toolkit`。
2. 运行：

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
.\install.ps1 -IncludeData
```

3. 打开本目录 `work-states\2026\codex-skills-mcp-github-sync_20260612-181402`。
4. 解压 `workspace.zip` 到新电脑的工作目录。
5. 进入解压后的 `CanVariableMonitor`，执行上面的编译和发布命令。

## 不要回退的点

- 不要恢复 `CanMonitor_Trace(...)` 自动插桩逻辑，老 C90/ARMCC 工程容易因为声明位置报错。
- 固件安装只做最小入口：补 `can_monitor_agent.c`、Keil 工程项、`CanMonitor_Process()` 调用。
- 客户界面不要暴露 PEAK/SYS/GC 选择，软件自动判断。
- 发布时继续保持 `win-x86`。
