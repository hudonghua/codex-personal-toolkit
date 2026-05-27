---
name: 记忆同步流程
description: 改 Claude 全局记忆时的标准同步路径——先改联想电脑本地仓库，再用脚本推 GitHub
type: feedback
---
# Claude 全局记忆同步流程

**规则：改 Claude 全局记忆（`memory/claude/*.md`）默认走联想电脑本地仓库 + sync-memory.ps1，不直接调 GitHub API。**

**Why:** 2026-05-27 用户提醒："你是在联想电脑，应该同步到联想电脑去。" 直接用 `gh api -X PUT` 改远端会绕过本机仓库，导致本地和远端分叉；同步基础设施（`sync-memory.ps1`）就是为这个场景准备的，应该用它。

**How to apply:**

## 标准路径（在联想电脑上）

1. **改本地仓库的文件**：
   `C:\Users\t250c\Documents\Codex\codex-personal-toolkit\memory\claude\<file>.md`
2. **运行同步脚本**：
   ```powershell
   powershell -NoProfile -ExecutionPolicy Bypass -File C:\Users\t250c\Documents\Codex\codex-personal-toolkit\sync-memory.ps1
   ```
   脚本会自动 `git pull --ff-only` → `git add memory` → `git commit -m "memory sync <ts>"` → `git push`。
3. **验证**：脚本输出 `memory: synced <时间戳>`。

## 何时可以直接 API（例外）

- 联想电脑本地仓库不可用（路径不存在、git 损坏）。
- 紧急情况下只有 `gh` 可用。
- **例外情况发生后必须立刻 pull 联想电脑本地仓库回到一致状态**，避免下次本地改动产生冲突。

## 禁止

- ❌ 用 `gh api -X PUT` 直接改 `memory/claude/` 远端文件而不同步联想电脑本地仓库
- ❌ 在联想电脑上改了本地文件后用别的方式 push（绕过 sync-memory.ps1 也行，但 commit message 要清楚）
- ❌ 同时在 API 端和本地端改同一文件造成分叉

## 关联
- 仓库：`hudonghua/codex-personal-toolkit`，master 分支
- 本地：`C:\Users\t250c\Documents\Codex\codex-personal-toolkit\`
- 同步脚本：`sync-memory.ps1`（memory 目录专用）、`sync-device-chats.ps1`（聊天记录用）
- Dell 电脑上对应路径需要 Dell 那台机器自己同步，联想电脑不能代它生成
