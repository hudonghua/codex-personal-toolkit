# Work State: codex-skills-mcp-github-sync_20260610-091029

This folder is a cross-computer Codex work-continuity snapshot.

## Contains

- `workspace.zip`: current workspace files, excluding cache/build/secrets patterns.
- `transcript.md`: readable chat transcript.
- `raw-session.jsonl`: raw local Codex session record.
- `manifest.json`: source paths and export metadata.
- `skill-list.txt`: installed skill list from the source computer.
- `mcp-summary.txt`: configured MCP servers summary.
- `mcp-data/`: local Memory/SQLite databases when available.

## Restore On Another Computer

1. Download or clone `hudonghua/codex-personal-toolkit`.
2. Run:

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
.\install.ps1 -IncludeData
```

3. Extract workspace:

```powershell
New-Item -ItemType Directory -Path $env:USERPROFILE\Documents\Codex\restored-workspaces -Force
Expand-Archive -Path .\work-states/2026/codex-skills-mcp-github-sync_20260610-091029\workspace.zip -DestinationPath $env:USERPROFILE\Documents\Codex\restored-workspaces
```

4. Open the restored workspace in Codex and continue.
5. Restart Codex if new skills/MCP servers were installed.

## Source

- Workspace: `E:\AI_划时代\全电脑_算法PCB\正确资料_算法PCB_CAN协议\MC_LCD - 7Control_V1.2\Codex_无缝交接_20260607-201828`
- Session: `C:\Users\t250c\.codex\sessions\2026\06\07\rollout-2026-06-07T20-24-45-019ea20a-edb7-7210-93af-c3b6e25b993f.jsonl`
- Exported: `2026-06-10T09:10:30`

## Added For This Upload

- `workspace.zip`: 当前 Arm200A 交接目录 `Codex_无缝交接_20260607-201828`。
- `codex-memories-current.zip`: 当前本机 Codex memories，包括 `MEMORY.md`、`memory_summary.md`、ad-hoc notes、rollout summaries，以及 Memory/SQLite 数据副本。
- `extra-current-files/推进梁XZ_YZ夹角函数_可复制.txt`: 推进梁 XZ/YZ 夹角可复制函数说明。
- `extra-current-files/V1_参考点.html`: 当前 V1 参考点/Body-World 验证计算器。
- `raw-session.jsonl` 和 `transcript.md` 已对 `sk-...` 形态误报字符串做脱敏替换。
