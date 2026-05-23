#!/usr/bin/env python3
"""Export the latest Codex chat session into a GitHub-ready archive folder."""

from __future__ import annotations

import argparse
import json
import os
import re
import shutil
from datetime import datetime
from pathlib import Path


SENSITIVE_PATTERNS = [
    re.compile(r"ghp_[A-Za-z0-9_]{20,}"),
    re.compile(r"github_pat_[A-Za-z0-9_]{20,}"),
    re.compile(r"sk-[A-Za-z0-9_-]{20,}"),
    re.compile(r"(?i)(password|passwd|token|secret|api[_-]?key)\s*[:=]\s*\S+"),
]


def latest_session(session_root: Path) -> Path:
    files = sorted(session_root.rglob("*.jsonl"), key=lambda p: p.stat().st_mtime, reverse=True)
    if not files:
        raise FileNotFoundError(f"No session jsonl files under {session_root}")
    return files[0]


def read_jsonl(path: Path) -> list[dict]:
    rows = []
    with path.open("r", encoding="utf-8", errors="replace") as handle:
        for line in handle:
            line = line.strip()
            if not line:
                continue
            try:
                rows.append(json.loads(line))
            except json.JSONDecodeError:
                rows.append({"type": "raw_line", "payload": {"text": line}})
    return rows


def text_from_content(content) -> str:
    if isinstance(content, str):
        return content
    if isinstance(content, list):
        parts = []
        for item in content:
            if isinstance(item, dict):
                if "text" in item:
                    parts.append(str(item["text"]))
                elif "input_text" in item:
                    parts.append(str(item["input_text"]))
            else:
                parts.append(str(item))
        return "\n".join(parts)
    return "" if content is None else str(content)


def extract_messages(rows: list[dict]) -> tuple[dict, list[tuple[str, str, str]]]:
    meta = {}
    messages: list[tuple[str, str, str]] = []
    seen = set()

    for row in rows:
        typ = row.get("type")
        payload = row.get("payload", {})
        ts = row.get("timestamp", "")

        if typ == "session_meta":
            meta = payload
            continue

        if typ == "response_item" and isinstance(payload, dict):
            if payload.get("type") == "message":
                role = payload.get("role", "message")
                text = text_from_content(payload.get("content"))
                key = (ts, role, text)
                if text and key not in seen:
                    seen.add(key)
                    messages.append((ts, role, text))

        elif typ == "event_msg" and isinstance(payload, dict):
            # Keep user/assistant visible messages that may not exist as response_item.
            if payload.get("type") == "user_message":
                role = "user"
                text = payload.get("message", "")
            elif payload.get("type") == "agent_message":
                role = "assistant"
                text = payload.get("message", "")
            else:
                continue
            key = (role, text)
            if text and key not in seen:
                seen.add(key)
                messages.append((ts, role, text))

    return meta, messages


def topic_from_messages(messages: list[tuple[str, str, str]]) -> str:
    text = "\n".join(m[2] for m in messages if m[1] == "user")
    candidates = [
        ("codex-skills-mcp-github-sync", ["skill", "MCP", "github", "上传"]),
        ("firmware-embedded-vision-skills", ["bin", "GPIO", "OpenCV", "嵌入式"]),
        ("chat-transcript-upload", ["聊天记录", "上传", "github"]),
    ]
    for slug, words in candidates:
        if any(w.lower() in text.lower() for w in words):
            return slug

    cleaned = re.sub(r"\s+", " ", text).strip()
    cleaned = re.sub(r"[^\w\u4e00-\u9fff -]+", "", cleaned)
    if not cleaned:
        return "chat-archive"
    # Keep ASCII-safe filenames more portable across tools.
    ascii_words = re.findall(r"[A-Za-z0-9]+", cleaned)
    if ascii_words:
        return "-".join(ascii_words[:8]).lower()
    return "chat-archive"


def contains_sensitive(raw_text: str) -> list[str]:
    found = []
    for pattern in SENSITIVE_PATTERNS:
        if pattern.search(raw_text):
            found.append(pattern.pattern)
    return found


def write_markdown(path: Path, source: Path, meta: dict, messages: list[tuple[str, str, str]]) -> None:
    title = path.parent.name
    lines = [
        f"# {title}",
        "",
        f"- Source session: `{source}`",
        f"- Session id: `{meta.get('id', '')}`",
        f"- Session cwd: `{meta.get('cwd', '')}`",
        f"- Exported at: `{datetime.now().isoformat(timespec='seconds')}`",
        "",
        "This Markdown file is a readable rendering. The adjacent raw `.jsonl` file is the exact archive.",
        "",
        "## Transcript",
        "",
    ]
    for ts, role, text in messages:
        lines.extend(
            [
                f"### {role} {ts}",
                "",
                "```text",
                text.rstrip(),
                "```",
                "",
            ]
        )
    path.write_text("\n".join(lines), encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--repo", default=r"C:\Users\t250c\Documents\Codex\codex-personal-toolkit")
    parser.add_argument("--session-root", default=str(Path.home() / ".codex" / "sessions"))
    parser.add_argument("--session-file")
    parser.add_argument("--allow-sensitive", action="store_true")
    args = parser.parse_args()

    repo = Path(args.repo)
    session_root = Path(args.session_root)
    session_file = Path(args.session_file) if args.session_file else latest_session(session_root)
    raw_text = session_file.read_text(encoding="utf-8", errors="replace")
    sensitive = contains_sensitive(raw_text)
    if sensitive and not args.allow_sensitive:
        print("Sensitive-looking content detected. Re-run with --allow-sensitive only if upload is intended.")
        for item in sensitive:
            print(f"- {item}")
        return 2

    rows = read_jsonl(session_file)
    meta, messages = extract_messages(rows)
    stamp = datetime.now().strftime("%Y%m%d-%H%M%S")
    year = datetime.now().strftime("%Y")
    topic = topic_from_messages(messages)
    archive_dir = repo / "chat-archives" / year / f"{topic}_{stamp}"
    archive_dir.mkdir(parents=True, exist_ok=False)

    md_path = archive_dir / "transcript.md"
    raw_path = archive_dir / session_file.name
    write_markdown(md_path, session_file, meta, messages)
    shutil.copy2(session_file, raw_path)

    manifest = {
        "source_session": str(session_file),
        "markdown": str(md_path),
        "raw_jsonl": str(raw_path),
        "exported_at": datetime.now().isoformat(timespec="seconds"),
        "message_count": len(messages),
    }
    (archive_dir / "manifest.json").write_text(
        json.dumps(manifest, ensure_ascii=False, indent=2),
        encoding="utf-8",
    )
    print(str(archive_dir))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
