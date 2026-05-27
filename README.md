# Codex Personal Toolkit

Personal Codex skills and local MCP servers for reuse across computers.

This repository is intended to sync reusable workflow knowledge, not Codex login state.

## Contents

- `skills/`: personal and installed global skills.
- `mcp_servers/`: local MCP server scripts.
- `mcp_config_snippets/`: config snippets that can be merged into `~/.codex/config.toml`.
- `records/`: optional exported records. Do not put secrets here.
- `install.ps1`: install this toolkit onto a computer.
- `backup.ps1`: refresh this toolkit from the current computer.

## Install On A New Windows Computer

Run from this repository:

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
.\install.ps1
```

Then restart Codex.

## Backup From Current Computer

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
.\backup.ps1
```

To also copy local Memory/SQLite databases:

```powershell
.\backup.ps1 -IncludeData
```

Only use `-IncludeData` if the records are safe to sync.

## Upload Work State

After installing this toolkit, saying `上传` to Codex should trigger the `work-continuity-sync` skill.

This creates a cross-computer work-state snapshot containing:

- current workspace zip
- readable transcript
- raw session `.jsonl`
- skill list
- MCP summary
- Memory/SQLite data when available
- restore instructions

Manual fallback:

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
.\upload-work.ps1
```

The upload writes to:

```text
work-states/YYYY/topic_YYYYMMDD-HHMMSS/
```

## Merge Another Computer

If another computer has its own toolkit export, copy or clone it locally, then run:

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
.\merge-toolkit.ps1 -Incoming "D:\path\to\other\codex-personal-toolkit"
```

The merge is non-destructive:

- new files are added
- identical files are skipped
- conflicting files are saved as `*.incoming-YYYYMMDD-HHMMSS.*`
- a report is written under `merge-reports/`

## Upload Current Chat Only

For chat transcript only, use:

Manual fallback:

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
.\upload-chat.ps1
```

The upload creates:

- `transcript.md`: readable transcript
- raw `.jsonl`: exact local Codex session archive
- `manifest.json`: source and export metadata

## Shared By Computer

This repository also supports per-computer shared folders such as `联想电脑/` and `dell电脑/`.

- `codex-chat-records/`: redacted Markdown exports of Codex chats
- `cursor-chat-records/`: redacted Markdown exports of Cursor chats
- `cursor-skills/`: Cursor skill snapshot for that computer
- `cursor-memory/`: Cursor memory Markdown files
- `prompt-notes/`: reusable prompt files for that computer

To sync the current Dell computer and rebuild the shared prompt pool:

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
.\sync-device-share.ps1
```

The script rebuilds `shared/latest-prompts/` by comparing `联想电脑/prompt-notes/` and `dell电脑/prompt-notes/`.
For the same relative path, the newer file wins and is pushed back to GitHub.

## Do Not Sync

Do not sync the whole `.codex` directory. It can contain login state, sessions, cache, local logs, and private data.
