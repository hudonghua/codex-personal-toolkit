# Cursor Agent 整合记忆

**生成日期：** 2026-05-10  
**用途：** 在 Cursor 中快速加载用户纪律与记忆入口；**权威细则仍以各源文件为准**，本文件为摘要与导航，避免与源文重复维护时漂移。

---

## 1. 信息来源（绝对路径）

| 来源 | 路径 |
|------|------|
| Claude 全局记忆索引 | `C:\Users\DELL\.claude\memory\MEMORY.md` |
| Claude 代码修改协议反馈 | `C:\Users\DELL\.claude\memory\feedback_code_modification_protocol.md` |
| Claude GB2312/GBK 修改方法 | `C:\Users\DELL\.claude\memory\feedback_gb2312_modification_method.md` |
| Claude 自定义技能位置 | `C:\Users\DELL\.claude\memory\user_custom_skills_location.md` |
| Codex Phase 2 记忆 | `C:\Users\DELL\.codex\memories\MEMORY.md` |
| Codex 用户侧摘要 | `C:\Users\DELL\.codex\memories\memory_summary.md` |
| Codex 原始记忆 | `C:\Users\DELL\.codex\memories\raw_memories.md` |
| 项目记忆总索引 | `C:\Users\DELL\.claude\projects\C--Users-DELL\memory\MEMORY.md` |
| **编程手册与纪律（必读权威）** | `C:\Users\DELL\.claude\projects\C--Users-DELL\memory\编程手册与纪律.md` |

**同步策略：** 当上述任一源文件更新（尤其是《编程手册与纪律》或 `MEMORY.md` 索引），应重新扫一眼本摘要是否仍准确，并更新「生成日期」或对应小节。

---

## 2. 最高优先级纪律（摘自《编程手册与纪律》，与 Claude 全局去重合并）

**权威全文：** `C:\Users\DELL\.claude\projects\C--Users-DELL\memory\编程手册与纪律.md`

### 2.1 文件与编码

- **改前备份**；禁止擅自改编码；不能把程序弄丢。（对应手册 §1.1、§8.1；Claude `feedback_code_modification_protocol` 强调高风险覆盖须用户确认。）
- **GBK/GB2312 的 C 源**：手册要求 **禁用 Edit 类工具改 GBK 文件**，用 **Python** 按字节读写：`decode('gbk', errors='ignore')` / `encode('gbk')`，一次性完成修改；改前可从备份恢复起点。（§1.2、§8.3）
- Claude 全局补充（`feedback_gb2312_modification_method`）：定位用 Grep；备份命名带时间戳；禁止易隐式转码的命令（如不当 `Set-Content`）；流程为 **读字节 → decode → 替换 → encode → 写回**。

### 2.2 编辑 C 函数边界

- **三步锁定**：锁函数头、锁对应结束 `}`、改动不越界；改完检查下一函数是否被误吞。（§1.3；与 `feedback_edit_function_safety` 一致。）

### 2.3 硬件与协议

- 硬件项目 **零马虎**；协议错则设备不工作；文档与结论须 **基于实际代码**，先读代码、可再验证 bin，禁止凭想象。（§2、§6.2）

### 2.4 嵌入式风格

- 优先简单变量，避免复杂指针/数组/枚举/结构体堆砌（便于 Keil 全局搜索）；命名跟用户习惯（§3）。

### 2.5 工作方式

- 遇阻 **自己找办法**（如 MCP 用 Python 抽 Access）；**追根溯源**，勿凭函数名臆断周期，须追到定时器/调用链（§4.1–4.2.1）。
- **真诚**：不确定说不确定；改后 **立即验证**，禁止敷衍式「已修好」（§4.3–4.4）。
- **先理解再动手**：先输出理解要点，用户确认后再写代码；注意周期与累加逻辑不要自作聪明（§4.5）。
- **最小改动**：只写必要代码，避免过度设计（§5.1）；200A 屏等项目跟既有套路（§5.2）。

### 2.6 文档与图表

- 图表：**清爽蓝色** 风格（§6.1）；文档基于实际代码与场景（§6.2）。

### 2.7 用户说「恢复记忆」时（手册 §7）

1. Read `编程手册与纪律.md`  
2. Read `C--Users-DELL\memory\MEMORY.md`  
3. 按任务再读 `project_*` / `reference_*` / `feedback_*`  
4. 向用户确认已读清单与可继续工作。

---

## 3. Claude 全局记忆（与上节已合并的要点）

- **修改代码前**：遵守系统提示中的安全与工具约束；**每个任务** 在开始前重温一次即可，勿重复刷；**必须备份**；高风险覆盖/删除须说明风险并取得 **明确确认**；勿用「绕过正规工具」的方式规避分块编辑。（`feedback_code_modification_protocol.md`）
- **自定义技能目录：** `C:\Users\DELL\.claude\skills`  
  重要技能名：`backup-and-edit`、`embedded-c-safe-edit`、`safe-edit-gbk`、`safe-restore`、`fix-braces`、`understand-first`、`verify-before-answer`。（`user_custom_skills_location.md`）

---

## 4. Codex 记忆工作区状态

- 截至整合时，`raw_memories.md` 内容为 **`No raw memories yet.`**，Phase 2 为 **空基线**。
- **不要**从 Codex 侧凭空推断用户长期偏好；待 `raw_memories.md`、`rollout_summaries/` 等出现真实任务证据后，应 **再跑一轮合并**，把新结论写入本文件或新小节。

---

## 5. 项目记忆导航（`C--Users-DELL\memory\MEMORY.md` 索引转载）

**索引权威副本：** `C:\Users\DELL\.claude\projects\C--Users-DELL\memory\MEMORY.md`  
**恢复记忆后须先读：** `C:\Users\DELL\.claude\projects\C--Users-DELL\memory\编程手册与纪律.md`

以下条目路径均为：`C:\Users\DELL\.claude\projects\C--Users-DELL\memory\<文件名>`

### Project Memories

- `project_qdn_full_computer.md` — 全电脑模型项目核心信息、文档位置、工作流程  
- `project_qdn_calibration_strategy.md` — 现场标定策略：两层标定、视觉自学习修正、倾角补偿机制  
- `project_qdn_work_scenario.md` — 实际工作场景：自动定位钻孔流程、炮孔图应用、操作界面设计  
- `project_qdn_vision_architecture.md` — 视觉系统架构：四点光源+solvePnP、实时性分析、第三层在电脑端实现  
- `project_mc_lcd_framework.md` — MC_LCD - 7Control_V1.2 项目框架：模块分工、正解计算、标定流程、HMI 交互  
- `project_qdn_vision_decisions.md` — 视觉系统架构与决策（2026-04-12）：定位、靶标、相机、标定链  
- `project_qdn_vision_calibration.md` — 视觉内参标定方案（Codex 版本）：自动筛帧、清晰度判定、样本管理  
- `project_qdn_t_shell_cam_implementation.md` — T_shell_cam 标定实现：C++ 函数库、标定流程、现场快速定位相机位姿  
- `project_200a_screen_engineering.md` — 200A 屏项目工程经验：MCU/屏端分工、代码风格、页面修改路径、首页画图规范  
- `project_drillplan_visualization.md` — 炮孔图生成：XML/Excel 数据源、坐标系转换、多视角 PNG/3D HTML、驾驶室操作手视角  
- `project_csxjqr_lcd_display.md` — C 除锈机器人发射机 LCD 界面：系统状态+6 路压力监控+状态报警，BF 变量控制刷新  

### Reference Materials

- `reference_200a_interface_definition.md` — 200A 屏接口定义：35 芯/8 芯插头针脚、DI/DO/PWM/CAN/模拟量分配  
- `reference_mcgspro_analysis.md` — McgsPro MCP 文件分析案例：华矿中型机、CAN 协议提取、28 变量、脚本统计  
- `reference_can_id_naming.md` — CAN ID 命名规则：EA05→0x2EA.05，变量名与 CAN 帧的映射关系  

### Feedback & Principles

- `feedback_backup_discipline.md` — 文件修改备份纪律：改前必备份（带时间戳），改坏不私自回退，退档需用户确认  
- `feedback_gbk_file_modification.md` — GBK 编码文件修改方法：只能用 Python 保持原编码，禁用 sed/Edit 工具  
- `feedback_brace_problem_fix.md` — 大括号问题修复方法：读程序分析逻辑，找真实配对关系，现场修复，绝不退档  
- `feedback_context_management.md` — 上下文管理约定：对话过长时主动提醒用户压缩，防止 API 400 错误  
- `feedback_problem_solving_attitude.md` — 工作态度：遇到技术障碍时自己想办法，不要停止  
- `feedback_deep_thinking.md` — 深入思考原则：主动分析本质，从现象反推原因，提供完整解决方案  
- `feedback_diagram_style.md` — 图表默认风格：所有图表使用清爽蓝色风格展示  
- `feedback_mcp_file_analysis.md` — MCP 文件分析方法：用 Python 提取 Access 数据库内容，不要直接放弃  
- `feedback_protocol_generation.md` — 协议文档生成规范：必须基于实际代码，不能凭想象，先读代码再验证 bin 文件  
- `feedback_sincerity_principle.md` — 真诚工作原则：不玩套路，不确定就说不确定，建立真正的信任关系  
- `feedback_hardware_project_rigor.md` — 硬件项目严谨性：不能有半点马虎，协议错了设备就不工作，只有对或错没有差不多  
- `feedback_file_encoding_disaster.md` — 文件编码处理底线：修改前必须备份，绝对不能把程序搞没了  
- `feedback_embedded_code_style.md` — 嵌入式代码风格：避免指针/数组/枚举/结构体，用简单变量方便 Keil 全局搜索追踪  
- `feedback_user_coding_standards.md` — 用户编码规范：变量命名、函数命名、200A 屏项目套路、已犯错误记录  
- `feedback_edit_function_safety.md` — 编辑函数安全流程：先锁函数头，再锁结束 `}`，只在函数内改，改完检查下一个函数  
- `feedback_bf_variable_pattern.md` — BF 变量双重数据源模式：正常运行从 CAN 读取，调试时手动修改，两者都需要  
- `feedback_bf_wl_compatibility.md` — BF 和 WL 变量兼容模式：条件赋值实现双重数据源共存，只要有一个非 0 就全部赋值  

---

## 6. 技能与 Cursor 规则入口（路径-only）

- **Claude 自定义技能：** `C:\Users\DELL\.claude\skills`  
- **Cursor 个人技能：** `C:\Users\DELL\.cursor\skills`  
- **Codex 技能：** `C:\Users\DELL\.codex\skills`（含 `g1joshi\opencv`）  
- **Cursor 侧技能（skills-cursor 等）：** `C:\Users\DELL\.cursor\skills-cursor`  
- **Agent 技能目录（含 memory skill）：** `C:\Users\DELL\.agents\skills`  
- **本整合文档：** `C:\Users\DELL\.cursor\memory\CURSOR_AGENT_MEMORY.md`  

### GBK 乱码注释修复（2026-05-23）

- `gbk-garbled-comments`：修复 GBK/GB2312 嵌入式 C/H 源码中 `??`、 `???`、乱码中文注释；路径 `C:\Users\DELL\.cursor\skills\gbk-garbled-comments\SKILL.md`.
- 触发场景：用户要求“把 ?? 改成中文”“清理乱码注释”“GBK 注释恢复”；必须先备份，用 Python 字节读写保持 GBK，只改注释不改业务逻辑，写中文时防止终端编码把中文变成 `?`，完成后做 GBK 解码扫描和 Keil 编译验证。

### OpenCV / QDN 视觉（2026-05-22 已安装，三端同步）

| Skill 名 | 用途 | Codex | Cursor | Claude |
|----------|------|-------|--------|--------|
| `opencv`（G1Joshi） | 基础 OpenCV：BGR、预处理、Canny、特征、视频、DNN 入门 | `.codex\skills\g1joshi\opencv\` | `.cursor\skills\opencv\` | `.claude\skills\opencv\` |
| `terminalskills-opencv` | 扩展：管线、轮廓、视频、DNN 示例 | `.codex\skills\terminalskills-opencv\` | 同左 | 同左 |
| `qdn-project-vision` | **本项目**：四点光斑、solvePnP、K/D、T_shell_cam；链到 `project_qdn_*` 记忆 | `.codex\skills\qdn-project-vision\` | 同左 | 同左 |

做 QDN/全电脑模型视觉任务时：**先 `qdn-project-vision`，再 `opencv` / `terminalskills-opencv`**。涉及 GBK/嵌入式 C 修改时仍优先 `embedded-c-safe-edit` / `safe-edit-gbk`。

---

## 7. 铁律速查（与手册 §九 对齐）

1. 修改前必须备份。  
2. GBK 编码 C 文件：用 Python 保持编码，不用 Edit 工具硬改。  
3. 改函数须锁定首尾边界，防误吞下一函数。  
4. 硬件/协议：严谨、以代码与证据为准。  
5. 嵌入式：简单变量、可搜索、跟用户命名习惯。  
6. 遇困自救、追踪到源头、诚实、改后验证。  
7. 先理解确认再写代码；改动最小化。  
8. 「恢复记忆」时按手册 §7 顺序读取并确认。  

**手册最后更新（源文）：** 2026-04-30，v1.0。

---

## 8. 粘贴到 Cursor「User Rules」的片段（全局生效）

**操作：** Cursor **Settings → Rules → User rules**（或等价入口），将下面 **英文** 整段粘贴保存。这样新开 Agent 时规则会进入系统提示，模型应按其中要求用 Read 工具拉取记忆文件。

```
At the start of every new Agent conversation — before modifying files or giving project-specific technical advice — use the read-file tool to load:
  C:\Users\DELL\.cursor\memory\CURSOR_AGENT_MEMORY.md

If the task involves editing code, embedded C, GB2312/GBK-encoded sources, hardware protocols, or industrial HMI, also read (in order):
  1) C:\Users\DELL\.claude\projects\C--Users-DELL\memory\编程手册与纪律.md
  2) C:\Users\DELL\.claude\projects\C--Users-DELL\memory\MEMORY.md
  Then open any task-relevant files listed in that MEMORY index (project_*, reference_*, feedback_*).

If the user says "恢复记忆" / restore memory, follow section 7 of 编程手册与纪律.md (read handbook, then MEMORY index, then task-specific notes) and confirm with the user when done.

For GBK/embedded workflows, prefer the user's skills under C:\Users\DELL\.claude\skills (e.g. embedded-c-safe-edit, safe-edit-gbk, backup-and-edit, verify-before-answer) when applicable.
```

**中文说明：** 规则本身用英文写，便于模型稳定解析；含义即——**新对话先读整合记忆；涉码/嵌入式/GBK/硬件时再读手册与索引；用户说恢复记忆则按手册 §7；技能目录优先走既有 SKILL 流程。**
