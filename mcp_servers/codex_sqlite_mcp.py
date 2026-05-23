#!/usr/bin/env python3
"""Small local MCP SQLite server for Codex.

Provides controlled access to a single SQLite database for structured notes.
"""

from __future__ import annotations

import json
import os
import sqlite3
import sys
import time
from pathlib import Path


CODEX_HOME = Path(os.environ.get("CODEX_HOME", str(Path.home() / ".codex")))
DB_PATH = Path(os.environ.get("CODEX_SQLITE_DB", CODEX_HOME / "mcp_data" / "records.sqlite"))


def db() -> sqlite3.Connection:
    DB_PATH.parent.mkdir(parents=True, exist_ok=True)
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    conn.execute(
        """
        CREATE TABLE IF NOT EXISTS records (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            category TEXT NOT NULL,
            title TEXT NOT NULL,
            body TEXT NOT NULL,
            tags TEXT NOT NULL DEFAULT '',
            created_at INTEGER NOT NULL,
            updated_at INTEGER NOT NULL
        )
        """
    )
    conn.commit()
    return conn


TOOLS = [
    {
        "name": "sqlite_query",
        "description": "Run a read-only SELECT query against the local records SQLite database.",
        "inputSchema": {
            "type": "object",
            "properties": {
                "sql": {"type": "string"},
                "params": {"type": "array", "items": {}},
                "limit": {"type": "integer", "minimum": 1, "maximum": 500},
            },
            "required": ["sql"],
        },
    },
    {
        "name": "sqlite_add_record",
        "description": "Add a structured record to the local records table.",
        "inputSchema": {
            "type": "object",
            "properties": {
                "category": {"type": "string"},
                "title": {"type": "string"},
                "body": {"type": "string"},
                "tags": {"type": "string"},
            },
            "required": ["category", "title", "body"],
        },
    },
    {
        "name": "sqlite_schema",
        "description": "Show tables and schemas in the local records SQLite database.",
        "inputSchema": {"type": "object", "properties": {}},
    },
]


def text_result(text: str, is_error: bool = False) -> dict:
    return {"content": [{"type": "text", "text": text}], "isError": is_error}


def call_tool(name: str, args: dict) -> dict:
    with db() as conn:
        if name == "sqlite_query":
            sql = args["sql"].strip()
            if not sql.lower().startswith(("select", "with", "pragma")):
                return text_result("Only read-only SELECT/WITH/PRAGMA queries are allowed.", True)
            limit = int(args.get("limit", 100))
            params = args.get("params", []) or []
            cur = conn.execute(sql, params)
            rows = cur.fetchmany(limit)
            return text_result(format_query(cur, rows))

        if name == "sqlite_add_record":
            now = int(time.time())
            cur = conn.execute(
                """
                INSERT INTO records(category, title, body, tags, created_at, updated_at)
                VALUES (?, ?, ?, ?, ?, ?)
                """,
                (
                    args["category"].strip(),
                    args["title"].strip(),
                    args["body"].strip(),
                    str(args.get("tags", "")).strip(),
                    now,
                    now,
                ),
            )
            conn.commit()
            return text_result(f"Inserted record id={cur.lastrowid}")

        if name == "sqlite_schema":
            rows = conn.execute(
                """
                SELECT name, sql
                FROM sqlite_master
                WHERE type IN ('table', 'index', 'view', 'trigger')
                ORDER BY type, name
                """
            ).fetchall()
            return text_result("\n\n".join(f"{r['name']}:\n{r['sql']}" for r in rows))

    return text_result(f"Unknown tool: {name}", True)


def format_query(cur: sqlite3.Cursor, rows: list[sqlite3.Row]) -> str:
    cols = [desc[0] for desc in cur.description or []]
    if not rows:
        return "No rows."
    lines = [" | ".join(cols), " | ".join("---" for _ in cols)]
    for row in rows:
        values = []
        for col in cols:
            value = row[col]
            text = "" if value is None else str(value)
            text = text.replace("\n", "\\n")
            if len(text) > 300:
                text = text[:300] + "..."
            values.append(text)
        lines.append(" | ".join(values))
    return "\n".join(lines)


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
                "serverInfo": {"name": "codex-sqlite", "version": "1.0.0"},
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
        except Exception as exc:
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
