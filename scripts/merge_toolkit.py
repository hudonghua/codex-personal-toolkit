#!/usr/bin/env python3
"""Merge another Codex toolkit folder into this toolkit without destructive overwrites."""

from __future__ import annotations

import argparse
import filecmp
import hashlib
import shutil
from datetime import datetime
from pathlib import Path


MERGE_DIRS = ["skills", "mcp_servers", "mcp_config_snippets", "work-states", "records"]
SKIP_NAMES = {".git", "__pycache__"}


def sha256(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            h.update(chunk)
    return h.hexdigest()


def copy_tree_merge(src: Path, dst: Path, stamp: str, report: list[str]) -> None:
    for item in src.rglob("*"):
        rel = item.relative_to(src)
        if any(part in SKIP_NAMES for part in rel.parts):
            continue
        target = dst / rel
        if item.is_dir():
            target.mkdir(parents=True, exist_ok=True)
            continue
        target.parent.mkdir(parents=True, exist_ok=True)
        if not target.exists():
            shutil.copy2(item, target)
            report.append(f"ADDED: {target}")
        elif filecmp.cmp(item, target, shallow=False):
            report.append(f"UNCHANGED: {target}")
        else:
            conflict = target.with_name(f"{target.stem}.incoming-{stamp}{target.suffix}")
            shutil.copy2(item, conflict)
            report.append(f"CONFLICT: kept {target}; incoming saved as {conflict}")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--incoming", required=True, help="Incoming toolkit folder from another computer.")
    parser.add_argument("--repo", default=r"C:\Users\t250c\Documents\Codex\codex-personal-toolkit")
    args = parser.parse_args()

    incoming = Path(args.incoming)
    repo = Path(args.repo)
    if not incoming.exists():
        raise FileNotFoundError(incoming)
    if not repo.exists():
        raise FileNotFoundError(repo)

    stamp = datetime.now().strftime("%Y%m%d-%H%M%S")
    report_lines = [
        f"# Toolkit Merge Report {stamp}",
        "",
        f"- Incoming: `{incoming}`",
        f"- Target: `{repo}`",
        "",
    ]

    for name in MERGE_DIRS:
        src = incoming / name
        dst = repo / name
        if not src.exists():
            report_lines.append(f"SKIP: incoming has no `{name}`")
            continue
        dst.mkdir(parents=True, exist_ok=True)
        report_lines.append(f"## {name}")
        copy_tree_merge(src, dst, stamp, report_lines)
        report_lines.append("")

    report_dir = repo / "merge-reports"
    report_dir.mkdir(parents=True, exist_ok=True)
    report_path = report_dir / f"{stamp}.md"
    report_path.write_text("\n".join(report_lines), encoding="utf-8")
    print(report_path)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
