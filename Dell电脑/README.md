# Dell电脑

这个目录用于共享 Dell 电脑上的 Codex 工作记录。

在 Dell 电脑上克隆本仓库后，运行：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\sync-device-chats.ps1 -DeviceName "Dell电脑"
```

脚本会先拉取 GitHub 最新内容，再导出 Dell 本机 Codex App 左侧栏聊天记录，最后提交并推送。

注意：联想电脑无法自动生成 Dell 电脑本机的聊天记录；Dell 目录需要在 Dell 电脑上同步，或者把 Dell 的 `.codex\sessions` 和 `session_index.jsonl` 带到当前电脑后再导出。
