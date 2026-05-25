---
name: shared-memory-layout-t250c
description: "How Claude, Cursor, and Codex share one memory store on the t250c machine via the codex-personal-toolkit GitHub repo"
metadata: 
  node_type: memory
  type: reference
  originSessionId: a73dd7fd-aeaf-4ca7-ad24-516e13570cdc
---

# Shared memory + skill layout (t250c machine)

This machine routes Claude Code, Cursor, and Codex memory through one GitHub-backed location instead of three separate stores. Set up on 2026-05-25.

## Source of truth

- GitHub repo: `https://github.com/hudonghua/codex-personal-toolkit` (private, account `hudonghua`)
- Local clone: `C:\Users\t250c\Documents\Codex\codex-personal-toolkit`
- Shared memory tree inside the clone:
  - `memory/claude/` — Claude memory (34 files restored from 20260525 DELL backup)
  - `memory/cursor/CURSOR_AGENT_MEMORY.md` — Cursor agent index

## Junctions (don't write to `.claude/`/`.cursor/` directly thinking it's local)

- `C:\Users\t250c\.claude\projects\C--Users-t250c\memory\` → `...\toolkit\memory\claude\`
- `C:\Users\t250c\.cursor\memory\` → `...\toolkit\memory\cursor\`

Writes through these paths land inside the toolkit working tree. The Stop hook in `~/.claude/settings.json` runs `toolkit\sync-memory.ps1`, which pulls, commits, and pushes any change under `memory/` after each turn.

## Tools (portable, on PATH via settings.json env)

- `C:\Users\t250c\.codex\tools\mingit\cmd\git.exe`
- `C:\Users\t250c\.codex\tools\gh\bin\gh.exe` — already authed as `hudonghua` with `repo, workflow, gist, read:org` scopes

## Skills

- `toolkit\skills\` has 40 skills, including `embedded-c-safe-edit`, `safe-edit-gbk`, `gbk-garbled-comments`, `qdn-project-vision`, `keil5-embedded-c`, `backup-and-edit`, `fix-braces`.
- These are the *cross-machine* copies. Claude's own skill loader still reads `~/.claude/skills/` and plugin skills — the toolkit skill set is a reference library, not auto-loaded by Claude Code.

## When the user says "restore memory"

1. Read `编程手册与纪律.md` (handbook + discipline) first.
2. Read `MEMORY.md` index.
3. Read task-relevant `project_*` / `reference_*` / `feedback_*` files.
4. Confirm with user what was recovered before continuing.

## Codex's own memory still lives at `.codex/memories/`

That tree (`MEMORY.md`, `memory_summary.md`, `programming_handbook_and_discipline.md`, `raw_memories.md`, `rollout_summaries/`) is Codex-format and stays where it is — it's not junctioned. For broad context on the user's environment, projects, and habits, that summary is the richest single file. Path: `C:\Users\t250c\.codex\memories\memory_summary.md`.
