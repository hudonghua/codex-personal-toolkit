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

## Do Not Sync

Do not sync the whole `.codex` directory. It can contain login state, sessions, cache, local logs, and private data.
