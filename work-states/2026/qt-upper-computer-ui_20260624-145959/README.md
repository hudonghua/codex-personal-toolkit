# Qt 上位机交接快照 20260624-145959

这是本次 Qt 上位机任务的安全交接快照。

## GitHub 源码

- 仓库：https://github.com/hudonghua/qt-upper-computer-ui
- 分支：main
- 提交：cdc72dbe2f442ed98a899cd2647f94515c7859c9

## 内容

- workspace.zip：由源码仓库 HEAD 生成的源码快照，不包含 build/release/.git/备份文件。
- TASK_MEMORY_20260624.md：本次任务关键记忆、业务规则和继续开发注意事项。
- 	ranscript.md：安全摘要，不含原始聊天敏感内容。
- aw-session.omitted.txt：说明为什么没有上传原始 session。
- skill-list.txt：本机 Codex skills 列表。
- mcp-summary.txt：本机 MCP 配置摘要，只列服务器表头和本地脚本路径。

## 恢复

`powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
.\install.ps1 -IncludeData
Expand-Archive -Path .\work-states\2026\qt-upper-computer-ui_20260624-145959\workspace.zip -DestinationPath C:\Users\t250c\Documents\restored-qt-upper-computer-ui -Force
`

然后打开恢复出的 Qt 工程，按 README 构建。
