#!/usr/bin/env python3
"""Sync one computer's Codex/Cursor records into the toolkit repo."""

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

EXCLUDED_NAMES = {
    ".git",
    ".system",
    "node_modules",
    ".venv",
    "venv",
    "__pycache__",
    ".cache",
}


def redact(text: str) -> str:
    for pattern, replacement in SENSITIVE_PATTERNS:
        text = pattern.sub(replacement, text)
    return text


def slugify(value: str, fallback: str = "untitled") -> str:
    cleaned = re.sub(r"\s+", "-", value.strip())
    cleaned = re.sub(r'[\\/:*?"<>|]+', "", cleaned)
    cleaned = cleaned.strip(".-")
    return (cleaned or fallback)[:80]


def reset_dir(path: Path) -> None:
    if path.exists():
        shutil.rmtree(path)
    path.mkdir(parents=True, exist_ok=True)


def copy_tree(source: Path, destination: Path, reset: bool = True) -> int:
    if reset and destination.exists():
        shutil.rmtree(destination)
    destination.mkdir(parents=True, exist_ok=True)
    count = 0
    if not source.exists():
        return count
    for item in source.iterdir():
        if item.name in EXCLUDED_NAMES:
            continue
        target = destination / item.name
        if item.is_dir():
            if target.exists():
                shutil.rmtree(target)
            shutil.copytree(item, target, ignore=shutil.ignore_patterns(*EXCLUDED_NAMES))
        else:
            shutil.copy2(item, target)
        count += 1
    return count


def write_text(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


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
                elif "content" in item:
                    parts.append(text_from_content(item["content"]))
            elif item:
                parts.append(str(item))
        return "\n".join(part for part in parts if part)
    if isinstance(content, dict):
        if "text" in content:
            return str(content["text"])
        if "content" in content:
            return text_from_content(content["content"])
    return "" if content is None else str(content)


def extract_codex_messages(rows: list[dict]) -> tuple[dict, list[tuple[str, str, str]]]:
    meta: dict = {}
    messages: list[tuple[str, str, str]] = []
    seen: set[tuple[str, str, str]] = set()
    for row in rows:
        typ = row.get("type")
        payload = row.get("payload", {})
        ts = row.get("timestamp", "")
        if typ == "session_meta" and isinstance(payload, dict):
            meta = payload
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
        elif typ == "response_item" and isinstance(payload, dict) and payload.get("type") == "message":
            role = payload.get("role", "message")
            text = text_from_content(payload.get("content"))
        text = redact(text.strip())
        if text:
            key = (ts, role, text)
            if key not in seen:
                seen.add(key)
                messages.append((ts, role, text))
    return meta, messages


def extract_cursor_messages(rows: list[dict]) -> list[tuple[str, str]]:
    messages: list[tuple[str, str]] = []
    seen: set[tuple[str, str]] = set()
    for row in rows:
        role = str(row.get("role", "")).strip() or "message"
        text = redact(text_from_content(row.get("message", {}).get("content", "")).strip())
        if text:
            key = (role, text)
            if key not in seen:
                seen.add(key)
                messages.append((role, text))
    return messages


def title_from_text(text: str, fallback: str) -> str:
    first_line = re.sub(r"<[^>]+>", " ", text).strip().splitlines()
    raw = first_line[0] if first_line else fallback
    raw = re.sub(r"\s+", " ", raw).strip()
    return raw[:60] or fallback


def export_codex(repo: Path, device_dir: Path, codex_home: Path) -> dict:
    out_root = device_dir / "codex-chat-records"
    reset_dir(out_root)
    session_root = codex_home / "sessions"
    session_files = sorted(session_root.rglob("*.jsonl"), key=lambda p: p.stat().st_mtime, reverse=True)
    exported = []
    for ordinal, session_file in enumerate(session_files[:80], start=1):
        rows = read_jsonl(session_file)
        meta, messages = extract_codex_messages(rows)
        if not messages:
            continue
        session_id = meta.get("id") or session_file.stem
        title = meta.get("title") or meta.get("cwd") or messages[0][2]
        title = title_from_text(str(title), session_id)
        filename = f"{ordinal:03d}_{session_id}_{slugify(title)}.md"
        lines = [
            f"# {title}",
            "",
            f"- Session id: `{session_id}`",
            f"- Source cwd: `{redact(str(meta.get('cwd', '')) )}`",
            f"- Exported at: `{datetime.now().isoformat(timespec='seconds')}`",
            f"- Local source: `{session_file}`",
            "",
            "## Transcript",
            "",
        ]
        for ts, role, text in messages:
            lines.extend([f"### {role} {ts}", "", "```text", text, "```", ""])
        write_text(out_root / filename, "\n".join(lines))
        exported.append(
            {
                "id": session_id,
                "title": title,
                "file": filename,
                "updated_at": datetime.fromtimestamp(session_file.stat().st_mtime).isoformat(timespec="seconds"),
                "message_count": len(messages),
            }
        )
    index_lines = [
        f"# {device_dir.name} Codex Chat Records",
        "",
        f"- Exported at: `{datetime.now().isoformat(timespec='seconds')}`",
        f"- Exported chats: `{len(exported)}`",
        "- Source: local Codex session `.jsonl`, rendered as redacted Markdown.",
        "",
        "## Chats",
        "",
    ]
    index_lines.extend(
        f"- [{item['title']}]({item['file']}) `{item['id']}` messages: `{item['message_count']}`"
        for item in exported
    )
    write_text(out_root / "index.md", "\n".join(index_lines) + "\n")
    write_text(
        out_root / "manifest.json",
        json.dumps(
            {
                "device": device_dir.name,
                "kind": "codex",
                "exported_at": datetime.now().isoformat(timespec="seconds"),
                "exported_count": len(exported),
                "exports": exported,
            },
            ensure_ascii=False,
            indent=2,
        ),
    )
    return {"count": len(exported), "path": str(out_root)}


def export_cursor(repo: Path, device_dir: Path, cursor_home: Path) -> dict:
    out_root = device_dir / "cursor-chat-records"
    reset_dir(out_root)
    transcript_files = sorted(
        [
            path
            for path in (cursor_home / "projects").rglob("*.jsonl")
            if "agent-transcripts" in path.parts and "subagents" not in path.parts
        ],
        key=lambda p: p.stat().st_mtime,
        reverse=True,
    )
    exported = []
    for ordinal, transcript in enumerate(transcript_files[:120], start=1):
        rows = read_jsonl(transcript)
        messages = extract_cursor_messages(rows)
        if not messages:
            continue
        first_user = next((text for role, text in messages if role == "user"), transcript.stem)
        title = title_from_text(first_user, transcript.stem)
        rel_file = transcript.relative_to(cursor_home)
        filename = f"{ordinal:03d}_{transcript.stem}_{slugify(title)}.md"
        lines = [
            f"# {title}",
            "",
            f"- Transcript id: `{transcript.stem}`",
            f"- Exported at: `{datetime.now().isoformat(timespec='seconds')}`",
            f"- Local source: `{transcript}`",
            f"- Cursor relative path: `{rel_file.as_posix()}`",
            "",
            "## Transcript",
            "",
        ]
        for role, text in messages:
            lines.extend([f"### {role}", "", "```text", text, "```", ""])
        write_text(out_root / filename, "\n".join(lines))
        exported.append(
            {
                "id": transcript.stem,
                "title": title,
                "file": filename,
                "updated_at": datetime.fromtimestamp(transcript.stat().st_mtime).isoformat(timespec="seconds"),
                "message_count": len(messages),
            }
        )
    index_lines = [
        f"# {device_dir.name} Cursor Chat Records",
        "",
        f"- Exported at: `{datetime.now().isoformat(timespec='seconds')}`",
        f"- Exported chats: `{len(exported)}`",
        "- Source: local Cursor `agent-transcripts/*.jsonl`, rendered as redacted Markdown.",
        "",
        "## Chats",
        "",
    ]
    index_lines.extend(
        f"- [{item['title']}]({item['file']}) `{item['id']}` messages: `{item['message_count']}`"
        for item in exported
    )
    write_text(out_root / "index.md", "\n".join(index_lines) + "\n")
    write_text(
        out_root / "manifest.json",
        json.dumps(
            {
                "device": device_dir.name,
                "kind": "cursor",
                "exported_at": datetime.now().isoformat(timespec="seconds"),
                "exported_count": len(exported),
                "exports": exported,
            },
            ensure_ascii=False,
            indent=2,
        ),
    )
    return {"count": len(exported), "path": str(out_root)}


def sync_prompt_notes(repo: Path, device_names: list[str]) -> dict:
    shared_root = repo / "shared" / "latest-prompts"
    reset_dir(shared_root)
    latest: dict[str, tuple[Path, float, str]] = {}
    for device_name in device_names:
        prompt_root = repo / device_name / "prompt-notes"
        prompt_root.mkdir(parents=True, exist_ok=True)
        for path in prompt_root.rglob("*"):
            if not path.is_file():
                continue
            rel = path.relative_to(prompt_root).as_posix()
            stamp = path.stat().st_mtime
            current = latest.get(rel)
            if current is None or stamp > current[1]:
                latest[rel] = (path, stamp, device_name)
    manifest_entries = []
    for rel, (source, stamp, device_name) in sorted(latest.items()):
        target = shared_root / rel
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, target)
        manifest_entries.append(
            {
                "relative_path": rel,
                "source_device": device_name,
                "source_file": str(source.relative_to(repo)).replace("\\", "/"),
                "updated_at": datetime.fromtimestamp(stamp).isoformat(timespec="seconds"),
            }
        )
    write_text(
        shared_root / "README.md",
        "# Shared Latest Prompts\n\n"
        "This folder is rebuilt from `联想电脑/prompt-notes` and `dell电脑/prompt-notes`.\n"
        "For files with the same relative path, the newer file wins.\n",
    )
    write_text(
        shared_root / "manifest.json",
        json.dumps(
            {
                "generated_at": datetime.now().isoformat(timespec="seconds"),
                "devices": device_names,
                "file_count": len(manifest_entries),
                "files": manifest_entries,
            },
            ensure_ascii=False,
            indent=2,
        ),
    )
    return {"count": len(manifest_entries), "path": str(shared_root)}


def sync_global_memory(repo: Path, device_names: list[str]) -> dict:
    shared_root = repo / "shared" / "global-memory"
    reset_dir(shared_root)
    latest: dict[str, tuple[Path, float, str]] = {}
    for device_name in device_names:
        candidates = [
            repo / device_name / "prompt-notes",
            repo / device_name / "cursor-memory",
            repo / device_name / "codex-memory",
        ]
        for base in candidates:
            if not base.exists():
                continue
            prefix = base.name
            for path in base.rglob("*"):
                if not path.is_file():
                    continue
                rel = f"{prefix}/{path.relative_to(base).as_posix()}"
                stamp = path.stat().st_mtime
                current = latest.get(rel)
                if current is None or stamp > current[1]:
                    latest[rel] = (path, stamp, device_name)
    manifest_entries = []
    for rel, (source, stamp, device_name) in sorted(latest.items()):
        target = shared_root / rel
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, target)
        manifest_entries.append(
            {
                "relative_path": rel,
                "source_device": device_name,
                "source_file": str(source.relative_to(repo)).replace("\\", "/"),
                "updated_at": datetime.fromtimestamp(stamp).isoformat(timespec="seconds"),
            }
        )
    write_text(
        shared_root / "README.md",
        "# Global Memory\n\n"
        "This folder is rebuilt from each computer's `prompt-notes`, `cursor-memory`, and `codex-memory`.\n"
        "For the same relative path inside the same category, the newer file wins.\n",
    )
    write_text(
        shared_root / "manifest.json",
        json.dumps(
            {
                "generated_at": datetime.now().isoformat(timespec="seconds"),
                "devices": device_names,
                "file_count": len(manifest_entries),
                "files": manifest_entries,
            },
            ensure_ascii=False,
            indent=2,
        ),
    )
    return {"count": len(manifest_entries), "path": str(shared_root)}


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--repo", required=True)
    parser.add_argument("--device-name", default="dell电脑")
    parser.add_argument("--peer-device", default="联想电脑")
    parser.add_argument("--codex-home", default=str(Path.home() / ".codex"))
    parser.add_argument("--cursor-home", default=str(Path.home() / ".cursor"))
    args = parser.parse_args()

    repo = Path(args.repo)
    device_dir = repo / args.device_name
    device_dir.mkdir(parents=True, exist_ok=True)

    codex_result = export_codex(repo, device_dir, Path(args.codex_home))
    cursor_result = export_cursor(repo, device_dir, Path(args.cursor_home))
    codex_skill_count = copy_tree(Path(args.codex_home) / "skills", repo / "skills", reset=False)
    cursor_skill_count = copy_tree(Path(args.cursor_home) / "skills", device_dir / "cursor-skills")
    cursor_memory_count = copy_tree(Path(args.cursor_home) / "memory", device_dir / "cursor-memory")
    codex_memory_count = copy_tree(Path(args.codex_home) / "memories", device_dir / "codex-memory")

    prompt_dir = device_dir / "prompt-notes"
    prompt_dir.mkdir(parents=True, exist_ok=True)
    if not (prompt_dir / "README.md").exists():
        write_text(
            prompt_dir / "README.md",
            "# Prompt Notes\n\n"
            "Place reusable prompt notes here. The sync script compares this folder with the same folder on the other computer and publishes the newer copy to `shared/latest-prompts/`.\n",
        )

    write_text(
        device_dir / "README.md",
        f"# {args.device_name}\n\n"
        "This folder stores shareable exports from this computer.\n\n"
        "- `codex-chat-records/`: redacted Markdown exports of local Codex chats\n"
        "- `codex-memory/`: local Codex memory snapshot\n"
        "- `cursor-chat-records/`: redacted Markdown exports of local Cursor chats\n"
        "- `cursor-skills/`: local Cursor skills snapshot\n"
        "- `cursor-memory/`: local Cursor memory Markdown files\n"
        "- `prompt-notes/`: user-provided prompt files to sync across computers\n",
    )

    prompt_result = sync_prompt_notes(repo, [args.peer_device, args.device_name])
    global_memory_result = sync_global_memory(repo, [args.peer_device, args.device_name])
    summary = {
        "device": args.device_name,
        "codex_chats": codex_result["count"],
        "codex_memory_files": codex_memory_count,
        "cursor_chats": cursor_result["count"],
        "codex_skills": codex_skill_count,
        "cursor_skills": cursor_skill_count,
        "cursor_memory_files": cursor_memory_count,
        "shared_prompt_files": prompt_result["count"],
        "shared_global_memory_files": global_memory_result["count"],
    }
    print(json.dumps(summary, ensure_ascii=False, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
