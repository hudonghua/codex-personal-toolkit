#!/usr/bin/env python3
"""Export Codex sidebar chats as sanitized Markdown files."""

from __future__ import annotations

import argparse
import json
import re
import shutil
from datetime import datetime
from pathlib import Path


SENSITIVE_PATTERNS = [
    (re.compile(r"sk-[A-Za-z0-9_-]{12,}"), "sk-***REDACTED***"),
    (re.compile(r"ghp_[A-Za-z0-9_]{20,}"), "ghp_***REDACTED***"),
    (re.compile(r"github_pat_[A-Za-z0-9_]{20,}"), "github_pat_***REDACTED***"),
    (
        re.compile(r"eyJ[A-Za-z0-9_-]{20,}\.[A-Za-z0-9_-]{20,}\.[A-Za-z0-9_-]{20,}"),
        "jwt.***REDACTED***",
    ),
    (
        re.compile(r"(?i)\b(password|passwd|token|secret|api[_-]?key)\s*[:=]\s*[^\s,;]+"),
        r"\1=***REDACTED***",
    ),
]


def redact(text: str) -> str:
    for pattern, replacement in SENSITIVE_PATTERNS:
        text = pattern.sub(replacement, text)
    return text


def slugify(value: str) -> str:
    cleaned = re.sub(r"\s+", "-", value.strip())
    cleaned = re.sub(r'[\\/:*?"<>|]+', "", cleaned)
    cleaned = cleaned.strip(".-")
    if not cleaned:
        return "untitled"
    return cleaned[:48]


def read_jsonl(path: Path) -> list[dict]:
    rows: list[dict] = []
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
                elif item.get("type") in {"input_text", "output_text"} and "text" in item:
                    parts.append(str(item["text"]))
            elif item:
                parts.append(str(item))
        return "\n".join(parts)
    return "" if content is None else str(content)


def extract_messages(rows: list[dict]) -> tuple[dict, list[tuple[str, str, str]]]:
    meta: dict = {}
    messages: list[tuple[str, str, str]] = []
    seen: set[tuple[str, str]] = set()

    for row in rows:
        typ = row.get("type")
        payload = row.get("payload", {})
        ts = row.get("timestamp", "")

        if typ == "session_meta":
            meta = payload if isinstance(payload, dict) else {}
            continue

        role = ""
        text = ""
        if typ == "event_msg" and isinstance(payload, dict):
            if payload.get("type") == "user_message":
                role = "user"
                text = payload.get("message", "")
            elif payload.get("type") == "agent_message":
                role = "assistant"
                text = payload.get("message", "")
        elif typ == "response_item" and isinstance(payload, dict):
            if payload.get("type") == "message":
                role = payload.get("role", "message")
                text = text_from_content(payload.get("content"))

        text = redact(text.strip())
        if text:
            key = (role, text)
            if key not in seen:
                seen.add(key)
                messages.append((ts, role, text))

    return meta, messages


def find_session_by_id(session_root: Path, session_id: str) -> Path | None:
    matches = [
        path
        for path in session_root.rglob(f"*{session_id}.jsonl")
        if ".bak" not in path.name
    ]
    if matches:
        return sorted(matches, key=lambda path: path.stat().st_mtime, reverse=True)[0]
    return None


def write_chat(path: Path, title: str, source: Path, meta: dict, messages: list[tuple[str, str, str]]) -> None:
    lines = [
        f"# {redact(title)}",
        "",
        f"- Session id: `{meta.get('id', '')}`",
        f"- Source cwd: `{redact(str(meta.get('cwd', '')) )}`",
        f"- Model provider: `{meta.get('model_provider', '')}`",
        f"- Exported at: `{datetime.now().isoformat(timespec='seconds')}`",
        f"- Local source: `{source}`",
        "",
        "## Transcript",
        "",
    ]

    for ts, role, text in messages:
        lines.extend([f"### {role} {ts}", "", "```text", text, "```", ""])

    path.write_text("\n".join(lines), encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--repo", required=True)
    parser.add_argument("--codex-home", default=str(Path.home() / ".codex"))
    parser.add_argument("--device-name", default="联想电脑")
    args = parser.parse_args()

    repo = Path(args.repo)
    codex_home = Path(args.codex_home)
    session_index = codex_home / "session_index.jsonl"
    session_root = codex_home / "sessions"
    out_root = repo / args.device_name / "chat-records"

    if not session_index.exists():
        raise FileNotFoundError(session_index)

    if out_root.exists():
        shutil.rmtree(out_root)
    out_root.mkdir(parents=True, exist_ok=True)

    sidebar_items = []
    for line in session_index.read_text(encoding="utf-8", errors="replace").splitlines():
        if not line.strip():
            continue
        sidebar_items.append(json.loads(line))

    exported = []
    missing = []
    for ordinal, item in enumerate(sidebar_items, start=1):
        session_id = item.get("id", "")
        title = item.get("thread_name") or session_id or "untitled"
        session_file = find_session_by_id(session_root, session_id)
        if not session_file:
            missing.append(item)
            continue
        rows = read_jsonl(session_file)
        meta, messages = extract_messages(rows)
        filename = f"{ordinal:03d}_{session_id}_{slugify(title)}.md"
        target = out_root / filename
        write_chat(target, title, session_file, meta, messages)
        exported.append(
            {
                "id": session_id,
                "title": redact(title),
                "file": str(target.relative_to(repo)).replace("\\", "/"),
                "updated_at": item.get("updated_at", ""),
                "message_count": len(messages),
            }
        )

    index_lines = [
        "# 联想电脑 Codex 聊天记录",
        "",
        f"- Exported at: `{datetime.now().isoformat(timespec='seconds')}`",
        f"- Exported chats: `{len(exported)}`",
        f"- Missing source files: `{len(missing)}`",
        "- Content policy: raw Codex `.jsonl` files are not uploaded; Markdown exports are redacted.",
        "",
        "## Chats",
        "",
    ]
    for item in exported:
        index_lines.append(
            f"- [{item['title']}]({Path(item['file']).name}) "
            f"`{item['id']}` messages: `{item['message_count']}`"
        )
    (out_root / "index.md").write_text("\n".join(index_lines) + "\n", encoding="utf-8")

    manifest = {
        "device": args.device_name,
        "exported_at": datetime.now().isoformat(timespec="seconds"),
        "exported_count": len(exported),
        "missing_count": len(missing),
        "exports": exported,
        "missing": missing,
    }
    (out_root / "manifest.json").write_text(
        json.dumps(manifest, ensure_ascii=False, indent=2),
        encoding="utf-8",
    )

    readme = repo / args.device_name / "README.md"
    readme.write_text(
        "# 联想电脑\n\n"
        "这个目录用于共享这台联想电脑上的 Codex 工作记录。\n\n"
        "- `chat-records/`: Codex App 左侧栏聊天记录的 Markdown 导出\n"
        "- 导出内容已做基础脱敏，不上传原始 `.jsonl`\n",
        encoding="utf-8",
    )

    print(json.dumps({"exported": len(exported), "missing": len(missing), "out": str(out_root)}, ensure_ascii=False))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
