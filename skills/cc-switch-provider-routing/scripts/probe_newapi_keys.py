#!/usr/bin/env python3
import argparse
import json
import sys
import urllib.error
import urllib.request
from pathlib import Path


def split_records(text: str) -> list[str]:
    records: list[str] = []
    depth = 0
    start = None
    for idx, ch in enumerate(text):
        if ch == "{":
            if depth == 0:
                start = idx
            depth += 1
        elif ch == "}":
            if depth == 0:
                continue
            depth -= 1
            if depth == 0 and start is not None:
                records.append(text[start : idx + 1])
                start = None
    return records


def fetch_models(base_url: str, key: str, timeout: int) -> tuple[int | None, str]:
    req = urllib.request.Request(
        base_url.rstrip("/") + "/v1/models",
        headers={"Authorization": f"Bearer {key}"},
        method="GET",
    )
    try:
        with urllib.request.urlopen(req, timeout=timeout) as resp:
            return resp.getcode(), resp.read().decode("utf-8", errors="replace")
    except urllib.error.HTTPError as exc:
        body = exc.read().decode("utf-8", errors="replace")
        return exc.code, body
    except Exception as exc:  # noqa: BLE001
        return None, str(exc)


def classify(models: list[str]) -> str:
    lowered = [m.lower() for m in models]
    if any(m.startswith("claude-") for m in lowered):
        return "claude"
    if any(m.startswith("gpt-") or "o1" == m or "o3" == m or "o4" == m for m in lowered):
        return "codex"
    return "unknown"


def main() -> int:
    parser = argparse.ArgumentParser(description="Probe newapi_channel_conn records and classify them for CC Switch.")
    parser.add_argument("file", help="Path to 密码.txt or another UTF-8 file containing blank-line-separated JSON records.")
    parser.add_argument("--timeout", type=int, default=30, help="HTTP timeout in seconds.")
    args = parser.parse_args()

    text = Path(args.file).read_text(encoding="utf-8")
    records = [json.loads(part) for part in split_records(text)]

    results = []
    for index, rec in enumerate(records, start=1):
        key = rec["key"]
        base_url = rec["url"]
        status, body = fetch_models(base_url, key, args.timeout)
        models: list[str] = []
        if status == 200:
            try:
                payload = json.loads(body)
                models = [item["id"] for item in payload.get("data", []) if isinstance(item, dict) and "id" in item]
            except json.JSONDecodeError:
                pass
        results.append(
            {
                "index": index,
                "key_preview": f"{key[:10]}...{key[-6:]}",
                "url": base_url,
                "http_status": status,
                "models": models,
                "classification": classify(models),
                "raw_body_preview": body[:300],
            }
        )

    json.dump(results, sys.stdout, ensure_ascii=False, indent=2)
    sys.stdout.write("\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
