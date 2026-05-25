# Cursor Agent Memory Index (t250c)

## ⚠️ 改代码前必读（强制 / Mandatory before editing code）

当用户提到 **修改 / 修复 / 改 / 调整** C/H 源码（尤其 Keil5 或 GBK 编码项目），
必须先按以下顺序**完整读取**（不是只看 description）：

1. `C:\Users\t250c\.codex\memories\programming_handbook_and_discipline.md` — 全局纪律
2. `C:\Users\t250c\Documents\Codex\codex-personal-toolkit\skills\embedded-c-safe-edit\SKILL.md` — **主流程（5 阶段）**
3. `...\toolkit\skills\backup-and-edit\SKILL.md` — 滚动 3 份备份
4. `...\toolkit\skills\safe-edit-gbk\SKILL.md` — GBK + 中文写入硬规则
5. `...\toolkit\skills\fix-braces\SKILL.md` — 大括号现场修复
6. `...\toolkit\skills\keil5-embedded-c\SKILL.md` — Keil5 总览

读完后**严格按 `embedded-c-safe-edit` 的 5 阶段流程执行**：

| 阶段 | 动作 |
|------|------|
| 1. 理解 | 打印理解（目标/文件/方案/执行周期/调用路径）→ 等用户确认 |
| 2. 备份 | 滚动 3 份 `.bak_YYYYMMDD_HHMMSS`，超 3 份删最旧 |
| 3. 修改 | GBK 文件用 Python 修改；**中文绝不进 PowerShell/cmd/`-c` 参数**；UTF-8 才可用 Edit |
| 4. 验证 | Python 重读 GBK，检查关键中文 Unicode 码点；不说"完成/完美" |
| 5. 报告 | 列已备份 / 已修改 / 已验证；让用户自己最终确认 |

### 铁律（违反任何一条都要停下来）
- 改前必备份（滚动 3 份）
- GBK 文件禁用 Edit / sed / PowerShell 传中文
- 函数边界三步锁定（头 + 结束 } + 范围内 + 检查下一函数）
- 大括号问题 **现场修复**，绝不退档
- 修改后立即验证；用户问"你确定？"必须重新查证据
- 退档需用户点头；坏版本另存 `.corrupted_<时间戳>` 不擦除

### 适用范围
- ✅ Keil5/uVision 项目的 C/H 源码（**最常见**）
- ✅ 任何 GBK 编码、含中文注释的嵌入式 C 文件
- ✅ MCU 协议代码（CAN/UART/RS485 frame、寄存器、定时器）
- ❌ 文档（.md/.txt 等 UTF-8 文本）不需要走此流程 — 文档由 git 版本控制

---

Read this file at the start of Agent sessions for user engineering tasks.

## Read by task type

| Task | Source |
|------|--------|
| Embedded / Keil / GBK / protocols | `C:\Users\t250c\.codex\memories\programming_handbook_and_discipline.md` |
| Restore memory / continuity | This file + `memory_summary.md` + rollout summaries |
| Upload / cross-machine sync | `C:\Users\t250c\.codex\skills\work-continuity-sync\SKILL.md` |
| Qt / OpenCV | `memory_summary.md` (Qt section); toolchain `E:\Qt\6.8.3\mingw_64` |
| AprilTag calibration | `memory_summary.md` (AprilTag section) |

## Full Codex memory

- `C:\Users\t250c\.codex\memories\MEMORY.md`
- `C:\Users\t250c\.codex\memories\memory_summary.md`
- `C:\Users\t250c\.codex\memories\rollout_summaries\`
- Skills: `C:\Users\t250c\.codex\skills\`

## Key paths

- Qt workspace: `E:\工作\QtOpenCVTemplate`, `E:\工作\QtSmokeTest`
- Toolkit repo: `hudonghua/codex-personal-toolkit`

## Restore-memory flow

1. Read programming_handbook_and_discipline.md
2. Read this index + memory_summary.md
3. Confirm recovered context with user, then continue

## Cursor rules

- User template: `C:\Users\t250c\.cursor\rules\*.mdc`
- Workspace copy: `E:\工作\.cursor\rules\*.mdc`