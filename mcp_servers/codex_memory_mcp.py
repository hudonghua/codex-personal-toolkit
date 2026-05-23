#!/usr/bin/env python3
"""Small local MCP memory server for Codex.

Stores practical notes in a SQLite database under CODEX_HOME/mcp_data.
It intentionally exposes only a few simple tools.
"""

from __future__ import annotations

import json
import os
import sqlite3
import sys
import time
from pathlib import Path


CODEX_HOME = Path(os.environ.get("CODEX_HOME", str(Path.home() / ".codex")))
DB_PATH = Path(os.environ.get("CODEX_MEMORY_DB", CODEX_HOME / "mcp_data" / "memory.sqlite"))


def db() -> sqlite3.Connection:
    DB_PATH.parent.mkdir(parents=True, exist_ok=True)
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    conn.execute(
        """
        CREATE TABLE IF NOT EXISTS memories (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            title TEXT NOT NULL,
            content TEXT NOT NULL,
            tags TEXT NOT NULL DEFAULT '',
            source TEXT NOT NULL DEFAULT '',
            created_at INTEGER NOT NULL,
            updated_at INTEGER NOT NULL
        )
        """
    )
    conn.execute(
        """
        CREATE VIRTUAL TABLE IF NOT EXISTS memories_fts
        USING fts5(title, content, tags, source, content='memories', content_rowid='id')
        """
    )
    conn.execute(
        """
        CREATE TRIGGER IF NOT EXISTS memories_ai AFTER INSERT ON memories BEGIN
            INSERT INTO memories_fts(rowid, title, content, tags, source)
            VALUES (new.id, new.title, new.content, new.tags, new.source);
        END
        """
    )
    conn.execute(
        """
        CREATE TRIGGER IF NOT EXISTS memories_ad AFTER DELETE ON memories BEGIN
            INSERT INTO memories_fts(memories_fts, rowid, title, content, tags, source)
            VALUES ('delete', old.id, old.title, old.content, old.tags, old.source);
        END
        """
    )
    conn.execute(
        """
        CREATE TRIGGER IF NOT EXISTS memories_au AFTER UPDATE ON memories BEGIN
            INSERT INTO memories_fts(memories_fts, rowid, title, content, tags, source)
            VALUES ('delete', old.id, old.title, old.content, old.tags, old.source);
            INSERT INTO memories_fts(rowid, title, content, tags, source)
            VALUES (new.id, new.title, new.content, new.tags, new.source);
        END
        """
    )
    conn.commit()
    return conn


TOOLS = [
    {
        "name": "memory_add",
        "description": "Save a reusable memory, lesson, workflow note, or user-provided record.",
        "inputSchema": {
            "type": "object",
            "properties": {
                "title": {"type": "string"},
                "content": {"type": "string"},
                "tags": {"type": "string", "description": "Comma-separated tags."},
                "source": {"type": "string", "description": "Where this memory came from."},
            },
            "required": ["title", "content"],
        },
    },
    {
        "name": "memory_search",
        "description": "Search saved memories by keyword.",
        "inputSchema": {
            "type": "object",
            "properties": {
                "query": {"type": "string"},
                "limit": {"type": "integer", "minimum": 1, "maximum": 50},
            },
            "required": ["query"],
        },
    },
    {
        "name": "memory_list",
        "description": "List recent saved memories.",
        "inputSchema": {
            "type": "object",
            "properties": {
                "limit": {"type": "integer", "minimum": 1, "maximum": 100},
                "tag": {"type": "string"},
            },
        },
    },
    {
        "name": "memory_delete",
        "description": "Delete one saved memory by id.",
        "inputSchema": {
            "type": "object",
            "properties": {"id": {"type": "integer"}},
            "required": ["id"],
        },
    },
]


def text_result(text: str, is_error: bool = False) -> dict:
    return {"content": [{"type": "text", "text": text}], "isError": is_error}


def call_tool(name: str, args: dict) -> dict:
    with db() as conn:
        if name == "memory_add":
            now = int(time.time())
            cur = conn.execute(
                """
                INSERT INTO memories(title, content, tags, source, created_at, updated_at)
                VALUES (?, ?, ?, ?, ?, ?)
                """,
                (
                    args["title"].strip(),
                    args["content"].strip(),
                    str(args.get("tags", "")).strip(),
                    str(args.get("source", "")).strip(),
                    now,
                    now,
                ),
            )
            conn.commit()
            return text_result(f"Saved memory id={cur.lastrowid}")

        if name == "memory_search":
            query = args["query"].strip()
            limit = int(args.get("limit", 10))
            rows = conn.execute(
                """
                SELECT m.id, m.title, m.content, m.tags, m.source, m.updated_at
                FROM memories_fts f
                JOIN memories m ON m.id = f.rowid
                WHERE memories_fts MATCH ?
                ORDER BY rank
                LIMIT ?
                """,
                (query, limit),
            ).fetchall()
            if not rows:
                return text_result("No memories found.")
            return text_result(format_rows(rows))

        if name == "memory_list":
            limit = int(args.get("limit", 20))
            tag = str(args.get("tag", "")).strip()
            if tag:
                rows = conn.execute(
                    """
                    SELECT id, title, content, tags, source, updated_at
                    FROM memories
                    WHERE tags LIKE ?
                    ORDER BY updated_at DESC
                    LIMIT ?
                    """,
                    (f"%{tag}%", limit),
                ).fetchall()
            else:
                rows = conn.execute(
                    """
                    SELECT id, title, content, tags, source, updated_at
                    FROM memories
                    ORDER BY updated_at DESC
                    LIMIT ?
                    """,
                    (limit,),
                ).fetchall()
            return text_result(format_rows(rows) if rows else "No memories saved yet.")

        if name == "memory_delete":
            cur = conn.execute("DELETE FROM memories WHERE id = ?", (int(args["id"]),))
            conn.commit()
            return text_result(f"Deleted {cur.rowcount} memory row(s).")

    return text_result(f"Unknown tool: {name}", True)


def format_rows(rows: list[sqlite3.Row]) -> str:
    parts = []
    for row in rows:
        content = row["content"]
        if len(content) > 800:
            content = content[:800] + "..."
        parts.append(
            f"ID: {row['id']}\nTitle: {row['title']}\nTags: {row['tags']}\n"
            f"Source: {row['source']}\nUpdated: {row['updated_at']}\nContent:\n{content}"
        )
    return "\n\n---\n\n".join(parts)


def handle(message: dict) -> dict | None:
    method = message.get("method")
    msg_id = message.get("id")

    if method and method.startswith("notifications/"):
        return None

    if method == "initialize":
        return {
            "jsonrpc": "2.0",
            "id": msg_id,
            "result": {
                "protocolVersion": "2024-11-05",
                "capabilities": {"tools": {}},
                "serverInfo": {"name": "codex-memory", "version": "1.0.0"},
            },
        }

    if method == "ping":
        return {"jsonrpc": "2.0", "id": msg_id, "result": {}}

    if method == "tools/list":
        return {"jsonrpc": "2.0", "id": msg_id, "result": {"tools": TOOLS}}

    if method == "tools/call":
        params = message.get("params", {})
        result = call_tool(params.get("name", ""), params.get("arguments", {}) or {})
        return {"jsonrpc": "2.0", "id": msg_id, "result": result}

    if method in ("resources/list", "prompts/list"):
        key = "resources" if method == "resources/list" else "prompts"
        return {"jsonrpc": "2.0", "id": msg_id, "result": {key: []}}

    return {
        "jsonrpc": "2.0",
        "id": msg_id,
        "error": {"code": -32601, "message": f"Method not found: {method}"},
    }


def main() -> None:
    db().close()
    for line in sys.stdin:
        if not line.strip():
            continue
        try:
            response = handle(json.loads(line))
        except Exception as exc:  # Keep server alive and surface errors to MCP client.
            response = {
                "jsonrpc": "2.0",
                "id": None,
                "error": {"code": -32000, "message": f"{type(exc).__name__}: {exc}"},
            }
        if response is not None:
            sys.stdout.write(json.dumps(response, ensure_ascii=False) + "\n")
            sys.stdout.flush()


if __name__ == "__main__":
    main()
