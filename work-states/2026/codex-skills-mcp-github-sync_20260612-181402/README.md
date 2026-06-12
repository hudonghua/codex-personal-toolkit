# Work State: codex-skills-mcp-github-sync_20260612-181402

This folder is a cross-computer Codex work-continuity snapshot.

## Latest CanVariableMonitor Note

Read `V1.20_UPDATE.md` first. It records the latest upper-computer release, the `0x16 00` bootloader finding, and the V1.20 downloader fix that disables blind `0x82` fast-mode probing for old bootloaders.

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
Expand-Archive -Path .\work-states/2026/codex-skills-mcp-github-sync_20260612-181402\workspace.zip -DestinationPath $env:USERPROFILE\Documents\Codex\restored-workspaces
```

4. Open the restored workspace in Codex and continue.
5. Restart Codex if new skills/MCP servers were installed.

## Source

- Workspace: `C:\Users\DELL\Documents\遥控器的杂碎事情\CanVariableMonitor`
- Session: `C:\Users\DELL\AppData\Local\Temp\codex_session_sanitized_20260612_181359.jsonl`
- Exported: `2026-06-12T18:14:06`
