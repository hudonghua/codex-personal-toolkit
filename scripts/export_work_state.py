#!/usr/bin/env python3
"""Export a cross-computer Codex work-state snapshot."""

from __future__ import annotations

import argparse
import json
import os
import re
import shutil
import zipfile
from datetime import datetime
from pathlib import Path

from export_chat_archive import extract_messages, latest_session, read_jsonl, topic_from_messages, write_markdown


SECRET_PATTERNS = [
    re.compile(r"ghp_[A-Za-z0-9_]{20,}"),
    re.compile(r"github_pat_[A-Za-z0-9_]{20,}"),
    re.compile(r"sk-[A-Za-z0-9_-]{32,}"),
    re.compile(r"-----BEGIN (RSA |OPENSSH |EC |DSA )?PRIVATE KEY-----"),
]

EXCLUDE_DIRS = {
    ".git",
    "node_modules",
    ".venv",
    "venv",
    "__pycache__",
    ".cache",
    ".pytest_cache",
    ".mypy_cache",
    "dist",
    "build",
    ".next",
    "target",
}

EXCLUDE_FILE_NAMES = {
    ".env",
    ".env.local",
    ".env.production",
    ".env.development",
}

EXCLUDE_SUFFIXES = {
    ".pem",
    ".key",
    ".pfx",
    ".p12",
    ".crt",
    ".cer",
    ".sqlite-shm",
    ".sqlite-wal",
}


def sensitive_hits(text: str) -> list[str]:
    hits = []
    for pattern in SECRET_PATTERNS:
        if pattern.search(text):
            hits.append(pattern.pattern)
    return hits


def latest_meta(rows: list[dict]) -> dict:
    for row in rows:
        if row.get("type") == "session_meta":
            return row.get("payload", {}) or {}
    return {}


def should_exclude(path: Path) -> bool:
    parts = set(path.parts)
    if parts & EXCLUDE_DIRS:
        return True
    if path.name in EXCLUDE_FILE_NAMES:
        return True
    if path.suffix.lower() in EXCLUDE_SUFFIXES:
        return True
    return False


def zip_workspace(workspace: Path, dest_zip: Path) -> tuple[int, int]:
    count = 0
    total = 0
    with zipfile.ZipFile(dest_zip, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=6) as zf:
        if workspace.is_file():
            zf.write(workspace, workspace.name)
            return 1, workspace.stat().st_size
        for file_path in workspace.rglob("*"):
            if not file_path.is_file():
                continue
            rel = file_path.relative_to(workspace)
            if should_exclude(rel):
                continue
            try:
                size = file_path.stat().st_size
            except OSError:
                continue
            # Avoid huge accidental binary dumps in continuity snapshots.
            if size > 100 * 1024 * 1024:
                continue
            zf.write(file_path, rel.as_posix())
            count += 1
            total += size
    return count, total


def write_skill_list(path: Path, codex_home: Path) -> None:
    skill_root = codex_home / "skills"
    lines = []
    for skill in sorted(skill_root.rglob("SKILL.md")):
        try:
            rel = skill.relative_to(skill_root)
        except ValueError:
            rel = skill
        lines.append(str(rel))
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def write_mcp_summary(path: Path, codex_home: Path) -> None:
    config = codex_home / "config.toml"
    servers = codex_home / "mcp_servers"
    lines = ["# MCP Summary", ""]
    if config.exists():
        text = config.read_text(encoding="utf-8", errors="replace")
        for line in text.splitlines():
            if line.startswith("[mcp_servers."):
                lines.append(line)
    lines.extend(["", "# Local MCP server scripts", ""])
    if servers.exists():
        for item in sorted(servers.glob("*")):
            lines.append(str(item))
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def copy_mcp_data(dest: Path, codex_home: Path) -> list[str]:
    src = codex_home / "mcp_data"
    copied = []
    if not src.exists():
        return copied
    dest.mkdir(parents=True, exist_ok=True)
    for item in src.glob("*.sqlite"):
        target = dest / item.name
        shutil.copy2(item, target)
        copied.append(str(target))
    return copied


def write_readme(path: Path, manifest: dict) -> None:
    restore_dir = r"$env:USERPROFILE\Documents\Codex\restored-workspaces"
    lines = [
        f"# Work State: {path.parent.name}",
        "",
        "This folder is a cross-computer Codex work-continuity snapshot.",
        "",
        "## Contains",
        "",
        "- `workspace.zip`: current workspace files, excluding cache/build/secrets patterns.",
        "- `transcript.md`: readable chat transcript.",
        "- `raw-session.jsonl`: raw local Codex session record.",
        "- `manifest.json`: source paths and export metadata.",
        "- `skill-list.txt`: installed skill list from the source computer.",
        "- `mcp-summary.txt`: configured MCP servers summary.",
        "- `mcp-data/`: local Memory/SQLite databases when available.",
        "",
        "## Restore On Another Computer",
        "",
        "1. Download or clone `hudonghua/codex-personal-toolkit`.",
        "2. Run:",
        "",
        "```powershell",
        "Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass",
        ".\\install.ps1 -IncludeData",
        "```",
        "",
        "3. Extract workspace:",
        "",
        "```powershell",
        f"New-Item -ItemType Directory -Path {restore_dir} -Force",
        f"Expand-Archive -Path .\\{path.parent.relative_to(path.parents[3]).as_posix()}\\workspace.zip -DestinationPath {restore_dir}",
        "```",
        "",
        "4. Open the restored workspace in Codex and continue.",
        "5. Restart Codex if new skills/MCP servers were installed.",
        "",
        "## Source",
        "",
        f"- Workspace: `{manifest.get('workspace')}`",
        f"- Session: `{manifest.get('session_file')}`",
        f"- Exported: `{manifest.get('exported_at')}`",
        "",
    ]
    path.write_text("\n".join(lines), encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--repo", default=r"C:\Users\t250c\Documents\Codex\codex-personal-toolkit")
    parser.add_argument("--codex-home", default=str(Path.home() / ".codex"))
    parser.add_argument("--session-root", default=str(Path.home() / ".codex" / "sessions"))
    parser.add_argument("--session-file")
    parser.add_argument("--workspace")
    parser.add_argument("--allow-sensitive", action="store_true")
    args = parser.parse_args()

    repo = Path(args.repo)
    codex_home = Path(args.codex_home)
    session_file = Path(args.session_file) if args.session_file else latest_session(Path(args.session_root))
    raw_text = session_file.read_text(encoding="utf-8", errors="replace")
    hits = sensitive_hits(raw_text)
    if hits and not args.allow_sensitive:
        print("Sensitive-looking token/private-key content detected; not exporting.")
        for hit in hits:
            print(f"- {hit}")
        return 2

    rows = read_jsonl(session_file)
    meta = latest_meta(rows)
    _, messages = extract_messages(rows)
    workspace = Path(args.workspace) if args.workspace else Path(meta.get("cwd") or os.getcwd())

    stamp = datetime.now().strftime("%Y%m%d-%H%M%S")
    year = datetime.now().strftime("%Y")
    topic = topic_from_messages(messages)
    out_dir = repo / "work-states" / year / f"{topic}_{stamp}"
    out_dir.mkdir(parents=True, exist_ok=False)

    transcript = out_dir / "transcript.md"
    raw_session = out_dir / "raw-session.jsonl"
    workspace_zip = out_dir / "workspace.zip"

    write_markdown(transcript, session_file, meta, messages)
    shutil.copy2(session_file, raw_session)
    file_count, total_bytes = zip_workspace(workspace, workspace_zip)
    write_skill_list(out_dir / "skill-list.txt", codex_home)
    write_mcp_summary(out_dir / "mcp-summary.txt", codex_home)
    mcp_files = copy_mcp_data(out_dir / "mcp-data", codex_home)

    manifest = {
        "workspace": str(workspace),
        "session_file": str(session_file),
        "exported_at": datetime.now().isoformat(timespec="seconds"),
        "topic": topic,
        "workspace_zip_files": file_count,
        "workspace_zip_source_bytes": total_bytes,
        "mcp_data_files": mcp_files,
    }
    (out_dir / "manifest.json").write_text(json.dumps(manifest, ensure_ascii=False, indent=2), encoding="utf-8")
    write_readme(out_dir / "README.md", manifest)
    print(str(out_dir))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
