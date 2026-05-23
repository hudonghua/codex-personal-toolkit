# Work State: codex-skills-mcp-github-sync_20260523-224944

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
Expand-Archive -Path .\work-states/2026/codex-skills-mcp-github-sync_20260523-224944\workspace.zip -DestinationPath $env:USERPROFILE\Documents\Codex\restored-workspaces
```

4. Open the restored workspace in Codex and continue.
5. Restart Codex if new skills/MCP servers were installed.

## Source

- Workspace: `C:\Users\t250c\Documents\Codex\2026-05-22\new-chat`
- Session: `C:\Users\t250c\.codex\sessions\2026\05\22\rollout-2026-05-22T01-23-38-019e4b90-74db-7e31-9c40-b40d5bcc9397.jsonl`
- Exported: `2026-05-23T22:49:44`
