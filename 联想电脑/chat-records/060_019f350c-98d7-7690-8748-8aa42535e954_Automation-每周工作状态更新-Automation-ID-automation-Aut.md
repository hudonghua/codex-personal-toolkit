# Automation: 每周工作状态更新 Automation ID: automation Automation memory: $CODEX_HOME/au

- Session id: `019f350c-98d7-7690-8748-8aa42535e954`
- Source cwd: `E:\工作`
- Model provider: `openai`
- Exported at: `2026-07-06T23:11:46`
- Local source: `C:\Users\t250c\.codex\sessions\2026\07\06\rollout-2026-07-06T09-30-50-019f350c-98d7-7690-8748-8aa42535e954.jsonl`

## Transcript

### developer 2026-07-06T01:30:57.225Z

```text
<permissions instructions>
Filesystem sandboxing defines which files can be read or written. `sandbox_mode` is `danger-full-access`: No filesystem sandboxing - all commands are permitted. Network access is enabled.
Approval policy is currently never. Do not provide the `sandbox_permissions` for any reason, commands will be rejected.
</permissions instructions>
Response MUST end with a remark-directive block.

## Responding

- Answer the user normally and concisely. Explain what you found, what you did, and what the user should focus on now.
- Automations: use the memory file at `$CODEX_HOME/automations/<automation_id>/memory.md` (create it if missing).
  - Read it first (if present) to avoid repeating recent work, especially for "changes since last run" tasks.
  - Memory is important: some tasks must build on prior work, and others must avoid duplicating prior focus.
  - Before returning the directive, write a concise summary of what you did/decided plus the current run time.
  - Use the `Automation ID:` value provided in the message to locate/update this file.
- REQUIRED: End with a valid remark-directive block on its own line (not inline).
  - Always include an inbox item directive:
    `::inbox-item{title="Sample title" summary="Place description here"}`

## Choosing return value

- For recurring/bg threads (e.g., "pull datadog logs and fix any new bugs", "address the PR comments"):
  - Always return `::inbox-item{...}` with the title/summary the user should see.

## Guidelines

- Directives MUST be on their own line.
- Output exactly ONE inbox-item directive.
- Do NOT use invalid remark-directive formatting.
- DO NOT place commas between arguments.
  - Valid: `::inbox-item{title="Sample title" summary="Place description here"}`
  - Invalid: `::inbox-item{title="Sample title",summary="Place description here"}`
- When referring to files, use full absolute filesystem links in Markdown (not relative paths).
  - Valid: [`/Users/alice/project/src/main.ts`](/Users/alice/project/src/main.ts)
  - Invalid: `src/main.ts` or `[main](src/main.ts)`
- Try not to ask the user for more input if possible to infer.
- If a PR is opened by the automation, add the `codex-automation` label when available alongside the normal `codex` label.
- Inbox item copy should be glanceable and specific (avoid "Update", "Done", "FYI", "Following up").
  - Title: what this thread now _is_ (state + object). Aim ~4-8 words.
  - Title should explain what was built or what happened.
- Summary: what the user should _do/know next_ (next step, blocker, or waiting-on). Aim ~6-14 words.
- Summary should usually match the general automation name or prompt summary.
- Both title and summary should be fairly short; usually avoid one-word titles/summaries.
  - Prefer concrete nouns + verbs; include a crisp status cue when helpful: "blocked", "needs decision", "ready for review".

## Examples (inbox-item)

- Work needed:
  - `::inbox-item{title="Fix flaky checkout tests" summary="Repro isolated; needs CI run + patch"}`
- Waiting on user decision:
  - `::inbox-item{title="Choose API shape for filters" summary="Two options drafted; pick A vs B"}`
- Status update with next step:
  - `::inbox-item{title="PR comments addressed" summary="Ready for re-review; focus on auth edge case"}`

<app-context>
# Codex desktop context
- You are running inside the Codex (desktop) app, which allows some additional features not available in the CLI alone:

### Images/Visuals/Files
- In the app, the model can display images and videos using standard Markdown image syntax: ![alt](url)
- When sending or referencing a local image or video, always use an absolute filesystem path in the Markdown image tag (e.g., ![alt](/absolute/path.png)); relative paths and plain text will not render the media.
- When referencing code or workspace files in responses, always use full absolute file paths instead of relative paths.
- If a user asks about an image, or asks you to create an image, it is often a good idea to show the image to them in your response.
- Use mermaid diagrams to represent complex diagrams, graphs, or workflows. Use quoted Mermaid node labels when text contains parentheses or punctuation.
- Return web URLs as Markdown links (e.g., [label](https://example.com)).

### Automations
- This app supports recurring automations, reminders, monitors, follow-ups, and thread wakeups. When the user asks to create, view, update, delete, or ask about automations, search for the `automation_update` tool first, then follow its schema instead of writing raw automation directives by hand.
- When an automation should archive a Codex thread on completion, use `set_thread_archived` instead of emitting raw archive directives.

### Thread Coordination
- When the user asks to create, fork, inspect, continue, hand off, pin, archive, rename, or otherwise manage Codex threads, search for the relevant thread tool first: `create_thread`, `fork_thread`, `list_threads`, `read_thread`, `send_message_to_thread`, `handoff_thread`, `set_thread_pinned`, `set_thread_archived`, or `set_thread_title`.
- Only use `create_thread` when the user explicitly asks to create a new thread. Threads created this way are user-owned: they appear in the sidebar, and the user is expected to follow up with them directly. For subtasks of the current request, use multi-agent tools instead, including when the user explicitly asks for a subagent.
- After a successful `create_thread` call, emit `::created-thread{threadId="..."}` for a created thread or `::created-thread{pendingWorktreeId="..."}` for queued worktree setup on its own line in your final response.

### Inline Code Comments
- Use the ::code-comment{...} directive when you need to attach feedback directly to specific code lines.
- Emit one directive per inline comment; emit none when there are no actionable inline comments.
- Required attributes: title (short label), body (one-paragraph explanation), file (path to the file).
- Optional attributes: start, end (1-based line numbers), priority (0-3).
- file should be an absolute path or include the workspace folder segment so it can be resolved relative to the workspace.
- Keep line ranges tight; end defaults to start.
- Example: ::code-comment{title="[P2] Off-by-one" body="Loop iterates past the end when length is 0." file="/path/to/foo.ts" start=10 end=11 priority=2}
</app-context>
<apps_instructions>
## Apps (Connectors)
Apps (Connectors) can be explicitly triggered in user messages in the format `[$app-name](app://{connector_id})`. Apps can also be implicitly triggered as long as the context suggests usage of available apps.
An app is equivalent to a set of MCP tools within the `codex_apps` MCP.
An installed app's MCP tools are either provided to you already, or can be lazy-loaded through the `tool_search` tool. If `tool_search` is available, the apps that are searchable by `tools_search` will be listed by it.
Do not additionally call list_mcp_resources or list_mcp_resource_templates for apps.
</apps_instructions>
<skills_instructions>
## Skills
A skill is a set of local instructions to follow that is stored in a `SKILL.md` file. Below is the list of skills that can be used. Each entry includes a name, description, and a short path that can be expanded into an absolute path using the skill roots table.
### Skill roots
- `r0` = `C:/Users/t250c/.codex/skills`
- `r1` = `C:/Users/t250c/.agents/skills`
- `r2` = `C:/Users/t250c/.codex/skills/.system`
- `r3` = `C:/Users/t250c/.codex/plugins/cache/openai-bundled`
- `r4` = `C:/Users/t250c/.codex/plugins/cache/openai-curated/github/3fdeeb49/skills`
- `r5` = `C:/Users/t250c/.codex/plugins/cache/openai-primary-runtime`
- `r6` = `C:/Users/t250c/.codex/plugins/cache/personal/cowart/0.1.3/skills`
### Available skills
- imagegen: Generate or edit raster images when the task benefits from AI-created bitmap visuals such as photos, illustrations, textures, sprites, mockups, or transparent-background cutouts. Use when Codex should create a brand-new image, transform an existing image, or derive visual variants from references, and the output should be a bitmap asset rather than repo-na (file: r2/imagegen/SKILL.md)
- openai-docs: Use when the user asks how to build with OpenAI products or APIs, asks about Codex itself or choosing Codex surfaces, needs up-to-date official documentation with citations, help choosing the latest model for a use case, or model upgrade and prompt-upgrade guidance; use OpenAI docs MCP tools for non-Codex docs questions, use the Codex manual helper first for (file: r2/openai-docs/SKILL.md)
- plugin-creator: Create and scaffold plugin directories for Codex with a required `.codex-plugin/plugin.json`, optional plugin folders/files, valid manifest defaults, and personal-marketplace entries by default. Use when Codex needs to create a new personal plugin, add optional plugin structure, generate or update marketplace entries for plugin ordering and availability me (file: r2/plugin-creator/SKILL.md)
- skill-creator: Guide for creating effective skills. This skill should be used when users want to create a new skill (or update an existing skill) that extends Codex's capabilities with specialized knowledge, workflows, or tool integrations. (file: r2/skill-creator/SKILL.md)
- skill-installer: Install Codex skills into $CODEX_HOME/skills from a curated list or a GitHub repo path. Use when a user asks to list installable skills, install a curated skill, or install a skill from another repo (including private repos). (file: r2/skill-installer/SKILL.md)
- arm-cortex-expert: ARM Cortex-M embedded development guidance for startup, vector table, NVIC, SysTick, low power, DMA, cache coherency, MPU, fault diagnosis, GPIO/peripheral registers, STM32/nRF/SAMD/RP2040-style MCUs, and real-time firmware debugging. (file: r0/arm-cortex-expert/SKILL.md)
- backup-and-edit: 修改文件前自动备份，并删除上一次备份（只保留最新一份）。适用于所有需要修改的文件，特别是C/H源码文件。 (file: r0/backup-and-edit/SKILL.md)
- browser:control-in-app-browser: Control the in-app Browser. Use to open, navigate, inspect, test, click, type, screenshot, or verify local targets such as localhost, 127.0.0.1, ::1, file://, the current in-app browser tab, and websites shown side by side inside Codex. (file: r3/browser/26.623.101652/skills/control-in-app-browser/SKILL.md)
- cc-switch-provider-routing: Use when working with CC Switch providers backed by third-party OpenAI-compatible or Claude-compatible gateways, especially when reading keys from E:\AI账号\密码.txt, classifying which models a key supports, adding keys to Codex or Claude in CC Switch, or fixing malformed provider settings_config JSON. (file: r0/cc-switch-provider-routing/SKILL.md)
- chat-transcript-uploader: Export local Codex chat/session records to timestamped documents and upload them to the user's GitHub toolkit repository. Use for explicit chat transcript/archive requests. For standalone "上传", prefer work-continuity-sync. (file: r0/chat-transcript-uploader/SKILL.md)
- computer-use:computer-use: Control Windows apps from Codex (file: r3/computer-use/26.623.101652/skills/computer-use/SKILL.md)
- cowart:cowart-image-edit: Generate new AI images from user-supplied Cowart annotation screenshots. Use when the user provides one or more screenshots showing Cowart images marked with the 批注 tool, arrows, or visible edit notes and wants Codex to apply those requested changes, create revised bitmap images, and place each result beside the corresponding original or in a nearby clear are (file: r6/cowart-image-edit/SKILL.md)
- cowart:cowart-image-gen: Generate a final AI bitmap for the Cowart canvas, including any requested in-image text by default. Use when the user asks Codex to create, fill, replace, or place an AI-generated image on a Cowart canvas. If an AI 图片 holder is selected, fill that holder; otherwise generate the image and insert it into the current Cowart page. (file: r6/cowart-image-gen/SKILL.md)
- cowart:cowart-open-canvas: Open the Cowart local web service, a tldraw-powered infinite canvas. Use when the user asks to open, launch, view, or work in the Cowart canvas or wants an infinite canvas inside Codex. (file: r6/cowart-open-canvas/SKILL.md)
- documents:documents: Create, edit, redline, and comment on `.docx`, Word, and Google Docs-targeted document artifacts inside the container, with a strict render-and-verify workflow. Use `render_docx.py` to generate page PNGs (and optional PDF) for visual QA, then iterate until layout is flawless before delivering the final document. (file: r5/documents/26.630.12135/skills/documents/SKILL.md)
- embedded-c-safe-edit: 安全修改嵌入式C代码（GBK编码）的完整工作流程。适用于修改老旧中文嵌入式项目的C/H文件，包含备份、编码保持、大括号修复、验证等全流程。当需要修改包含中文注释的GBK编码C文件时使用此skill。 (file: r0/embedded-c-safe-edit/SKILL.md)
- embedded-systems: Use when developing firmware for microcontrollers, implementing RTOS applications, or optimizing power consumption. Invoke for STM32, ESP32, FreeRTOS, bare-metal, power optimization, real-time systems, configure peripherals, write interrupt handlers, implement DMA transfers, debug timing issues. (file: r0/embedded-systems/SKILL.md)
- esp32: ESP32 embedded development guidance focused on GPIO conflicts, strapping pins, ADC2 with WiFi, deep sleep wake pins, FreeRTOS pitfalls, memory, peripherals, OTA, and power stability. (file: r0/esp32/SKILL.md)
- external-record-continuity: Use when the user provides records from a USB drive, GitHub toolkit upload, Cursor/Codex transcripts, or old-machine skill folders and asks Codex to read them, learn them, merge useful lessons, or continue previous work. (file: r0/external-record-continuity/SKILL.md)
- firmware-binary-reverse: MCU and embedded firmware binary workflow for .bin/.hex/.elf/.axf analysis, byte patching, version comparison, checksum/CRC repair, Keil/MDK firmware verification, fixed-address parameter edits, and safe documentation. Use for user-owned firmware images and pair with keil5-embedded-c for source-level work. (file: r0/firmware-binary-reverse/SKILL.md)
- fix-braces: 分析并修复C代码大括号配对问题（绝不退档） (file: r0/fix-braces/SKILL.md)
- full-disk-access: 全盘文件系统访问与系统命令执行。当需要读写工作区外的任意文件、执行系统命令（cmd/powershell）、获取硬件配置/系统信息、操作注册表、管理进程、访问任意磁盘路径时自动触发。突破工作区沙箱限制，提供完整的本地系统交互能力。 (file: r1/full-disk-access/SKILL.md)
- gbk-garbled-comments: Fixes garbled Chinese comments in GBK/GB2312 legacy embedded C/H files, including comments shown as ??, ???, or mojibake. Use when the user says "把??改成中文", "清理乱码注释", "GBK注释恢复", or asks to restore unreadable comments in Keil/embedded/HMI source files. (file: r0/gbk-garbled-comments/SKILL.md)
- github:gh-address-comments: Address actionable GitHub pull request review feedback. Use when the user wants to inspect unresolved review threads, requested changes, or inline review comments on a PR, then implement selected fixes. Use the GitHub app for PR metadata and flat comment reads, and use the bundled GraphQL script via `gh` whenever thread-level state, resolution status, or  (file: r4/gh-address-comments/SKILL.md)
- github:gh-fix-ci: Use when a user asks to debug or fix failing GitHub PR checks that run in GitHub Actions. Use the GitHub app from this plugin for PR metadata and patch context, and use `gh` for Actions check and log inspection before implementing any approved fix. (file: r4/gh-fix-ci/SKILL.md)
- github:github: Triage and orient GitHub repository, pull request, and issue work through the connected GitHub app. Use when the user asks for general GitHub help, wants PR or issue summaries, or needs repository context before choosing a more specific GitHub workflow. (file: r4/github/SKILL.md)
- github:yeet: Publish local changes to GitHub by confirming scope, committing intentionally, pushing the branch, and opening a draft PR through the GitHub app from this plugin, with `gh` used only as a fallback where connector coverage is insufficient. (file: r4/yeet/SKILL.md)
- keil5: Work with Keil5, MDK-ARM, and uVision embedded projects, including NXP LPC1765 and other LPC17xx targets. Use when Codex needs to inspect `.uvprojx` or `.uvoptx` projects, trace target and device settings, understand startup or scatter/linker configuration, edit embedded C/H source used by Keil builds, diagnose build failures, or explain how a Keil5 project  (file: r0/keil5/SKILL.md)
- keil5-embedded-c: Keil5 and embedded C firmware workflow for hardware-related work. Use when Codex works on Keil/uVision projects, C51/ARM/MCU/单片机 code, GBK C files, timers/定时器, interrupts/中断, IO/ADC/PWM/CAN/UART/RS485 protocols/硬件协议, screen/屏端 integration, bin/hex verification, protocol documents/协议文档, or embedded C code changes where correctness depends on real code paths (file: r0/keil5-embedded-c/SKILL.md)
- mcu-gpio-peripheral: MCU GPIO and peripheral bring-up workflow for embedded C/Keil/MDK projects. Covers input/output, pullup/pulldown, push-pull/open-drain, alternate functions, EXTI interrupts, debounce, ADC/PWM/UART/I2C/SPI pin conflicts, schematics, register/HAL checks, and Chinese embedded-code conventions. (file: r0/mcu-gpio-peripheral/SKILL.md)
- memory: Load consolidated user memory for Cursor — discipline summary, Claude/Codex status, and links to project memory under .claude/projects/C--Users-DELL/memory. (file: r0/memory/SKILL.md)
- multi-computer-toolkit-merge: Merge Codex toolkit uploads from multiple computers. Use when another computer has its own skills, MCP servers, work-states, memory/sqlite records, or task snapshots that need to be integrated without overwriting local work. (file: r0/multi-computer-toolkit-merge/SKILL.md)
- opencv: OpenCV computer vision library. Use for image processing. (file: r0/g1joshi/opencv/SKILL.md)
- opencv: OpenCV computer vision library. Use for image processing. (file: r0/g1joshi-opencv/SKILL.md)
- opencv: OpenCV computer vision library. Use for image processing. (file: r0/opencv/SKILL.md)
- opencv: OpenCV computer vision library. Use for image processing. (file: r0/opencv-g1joshi/SKILL.md)
- opencv: You are an expert in OpenCV (Open Source Computer Vision Library), the most popular library for real-time computer vision. You help developers build image processing pipelines, object detection systems, video analysis tools, augmented reality, and document processing using OpenCV's 2,500+ algorithms for image manipulation, feature detection, camera calibrat (file: r0/opencv-terminalskills/SKILL.md)
- pdf: Use when tasks involve reading, creating, or reviewing PDF files where rendering and layout matter; prefer visual checks by rendering pages (Poppler) and use Python tools such as `reportlab`, `pdfplumber`, and `pypdf` for generation and extraction. (file: r0/pdf/SKILL.md)
- pdf:pdf: Read, create, inspect, render, and verify PDF files where visual layout matters. Use Poppler rendering plus Python tools such as reportlab, pdfplumber, and pypdf for generation and extraction. (file: r5/pdf/26.630.12135/skills/pdf/SKILL.md)
- pptx-direct-edit: Directly edit PPTX slides without installing python-pptx. Use when the user asks to redesign tables, change colors, adjust layouts, or modify any PPTX content. Fast path via zipfile + ElementTree Open XML manipulation. Targets 10-second turnaround for single-slide table redesigns. (file: r1/pptx-direct-edit/SKILL.md)
- presentations:Presentations: Create or edit PowerPoint or Google Slides decks (file: r5/presentations/26.630.12135/skills/presentations/SKILL.md)
- ps: Use when the user says "ps", "图片", or asks to optimize, redesign, modularize, layer, split, or export Photoshop/PSD/image-based HMI screens where components must independently show/hide. Especially for industrial equipment UI PSDs, button bars, parameter pages, and Photoshop-compatible component layers. (file: r0/ps/SKILL.md)
- qdn-project-vision: QDN / 全电脑模型工业视觉项目上下文。涉及四点 LED 光斑、solvePnP 位姿、K/D 内参、 T_shell_cam 标定、QT+OpenCV C++ 工控端视觉时必读。与通用 opencv skill 配合使用。 (file: r0/qdn-project-vision/SKILL.md)
- qt-upper-computer-ui-layout: Use when working on the user's Qt/C++ industrial upper-computer UI, especially qt-upper-computer-ui, P02/P07/P08/P10 pages, status pages, parameter pages, touch-screen layouts, siton-style screens, or CAN-backed HMI data displays that need worker-facing layout and fixed local verification. (file: r0/qt-upper-computer-ui-layout/SKILL.md)
- radare2: Radare2 reverse-engineering workflow for binary analysis, disassembly, function discovery, strings, cross references, binary diffing, and safe patch planning. Use when analyzing or patching executables, firmware blobs, ELF/PE/Mach-O files, or raw binaries with r2/rabin2/rahash2/radiff2/rasm2. (file: r0/radare2/SKILL.md)
- reverse_engineer: Systematic binary reverse engineering toolkit. Extract source code, understand functions, document protocols, compare versions. Uses strings, symbols, disassembly, and differential verification. (file: r0/reverse_engineer/SKILL.md)
- safe-collab-workflow: Safe collaborative code-edit workflow for shared workspaces. Use when Codex needs to edit, repair, or verify code/files safely, especially for legacy C/H projects, GBK or Chinese-comment source files, brace or scope issues that should be fixed in place, backup or restore decisions, timing-sensitive logic that must be traced before changing, or any task whe (file: r0/safe-collab-workflow/SKILL.md)
- safe-edit-gbk: 安全修改GBK编码的C文件（自动备份+Python修改+验证） (file: r0/safe-edit-gbk/SKILL.md)
- safe-restore: 安全的文件恢复流程（必须用户确认） (file: r0/safe-restore/SKILL.md)
- security-best-practices: Perform language and framework specific security best-practice reviews and suggest improvements. Trigger only when the user explicitly requests security best practices guidance, a security review/report, or secure-by-default coding help. Trigger only for supported languages (python, javascript/typescript, go). Do not trigger for general code review, debuggin (file: r0/security-best-practices/SKILL.md)
- security-threat-model: Repository-grounded threat modeling that enumerates trust boundaries, assets, attacker capabilities, abuse paths, and mitigations, and writes a concise Markdown threat model. Trigger only when the user explicitly asks to threat model a codebase or path, enumerate threats/abuse paths, or perform AppSec threat modeling. Do not trigger for general architecture  (file: r0/security-threat-model/SKILL.md)
- senior-computer-vision: Computer vision engineering skill for object detection, image segmentation, and visual AI systems. Covers CNN and Vision Transformer architectures, YOLO/Faster R-CNN/DETR detection, Mask R-CNN/SAM segmentation, and production deployment with ONNX/TensorRT. Includes PyTorch, torchvision, Ultralytics, Detectron2, and MMDetection frameworks. Use when building (file: r0/senior-computer-vision/SKILL.md)
- spreadsheets:Spreadsheets: Use this skill when a user requests to create, modify, analyze, visualize, or work with spreadsheet files (`.xlsx`, `.xls`, `.csv`, `.tsv`) or Google Sheets-targeted spreadsheet artifacts with formulas, formatting, charts, tables, and recalculation. (file: r5/spreadsheets/26.630.12135/skills/spreadsheets/SKILL.md)
- survey-data-processor: Survey data processing for total-station, GNSS, leveling, point clouds, DTM/TIN generation, coordinate transformation, traverse adjustment, level-loop adjustment, contour generation, and survey deliverables. Use when processing field survey observations, control points, adjusted coordinates, or measurement workbooks. (file: r0/survey-data-processor/SKILL.md)
- template-creator:template-creator: Create or update a reusable personal Codex artifact-template skill. Use when the user invokes $template-creator or asks in natural language to create a template using, from, or based on an attached Word document, PowerPoint presentation, or Excel workbook, or explicitly asks to edit or update a passed artifact-template skill. Do not use for one-off artifact  (file: r5/template-creator/26.630.12135/skills/template-creator/SKILL.md)
- total-station-calibration: Total station calibration and error-analysis workflow. Use for total-station instrument checks, collimation error, trunnion-axis error, vertical-index error, EDM additive/multiplicative constants, prism constant, atmospheric correction, two-face observations, resection checks, calibration records, and Excel-based calibration calculations. (file: r0/total-station-calibration/SKILL.md)
- understand-anything:understand: Analyze a codebase to produce an interactive knowledge graph for understanding architecture, components, and relationships (file: C:/Users/t250c/.understand-anything/repo/understand-anything-plugin/skills/understand/SKILL.md)
- understand-anything:understand-chat: Use when you need to ask questions about a codebase or understand code using a knowledge graph (file: C:/Users/t250c/.understand-anything/repo/understand-anything-plugin/skills/understand-chat/SKILL.md)
- understand-anything:understand-dashboard: Launch the interactive web dashboard to visualize a codebase's knowledge graph (file: C:/Users/t250c/.understand-anything/repo/understand-anything-plugin/skills/understand-dashboard/SKILL.md)
- understand-anything:understand-diff: Use when you need to analyze git diffs or pull requests to understand what changed, affected components, and risks (file: C:/Users/t250c/.understand-anything/repo/understand-anything-plugin/skills/understand-diff/SKILL.md)
- understand-anything:understand-domain: Extract business domain knowledge from a codebase and generate an interactive domain flow graph. Works standalone (lightweight scan) or derives from an existing /understand knowledge graph. (file: C:/Users/t250c/.understand-anything/repo/understand-anything-plugin/skills/understand-domain/SKILL.md)
- understand-anything:understand-explain: Use when you need a deep-dive explanation of a specific file, function, or module in the codebase (file: C:/Users/t250c/.understand-anything/repo/understand-anything-plugin/skills/understand-explain/SKILL.md)
- understand-anything:understand-knowledge: Analyze a Karpathy-pattern LLM wiki knowledge base and generate an interactive knowledge graph with entity extraction, implicit relationships, and topic clustering. (file: C:/Users/t250c/.understand-anything/repo/understand-anything-plugin/skills/understand-knowledge/SKILL.md)
- understand-anything:understand-onboard: Use when you need to generate an onboarding guide for new team members joining a project (file: C:/Users/t250c/.understand-anything/repo/understand-anything-plugin/skills/understand-onboard/SKILL.md)
- understand-first: 先理解任务再执行，打印理解内容等用户确认 (file: r0/understand-first/SKILL.md)
- univer-cli: Use when solving spreadsheet workbook problems with the `univer` or `unv` CLI as a terminal-native spreadsheet engine: Excel-compatible `.xlsx` handoff, `.univer` or `.unv` packages, workbook inspection, range search, formulas, formatting, charts, shapes, floating images, rich spreadsheet edits, live preview and viewer review comments, versioning, shell-na (file: r0/univer-cli/SKILL.md)
- verify-before-answer: 修改代码后立即验证，不敷衍用户 (file: r0/verify-before-answer/SKILL.md)
- visual-plan: Turn ordinary text plans into rich interactive visual plans with diagrams, file maps, annotated code, open questions, and UI/prototype review when useful. (file: r1/visual-plan/SKILL.md)
- visual-plan: Turn ordinary text plans into rich interactive visual plans with diagrams, file maps, annotated code, open questions, and UI/prototype review when useful. (file: r0/visual-plan/SKILL.md)
- work-continuity-sync: Triggered when the user says "上传" as a standalone command or asks for seamless work handoff across computers. Create and upload a complete work-state snapshot including current workspace, session transcript/raw record, skills/MCP/toolkit state, Memory/SQLite records when available, and restore instructions. (file: r0/work-continuity-sync/SKILL.md)
- workflow-memory-skillsmith: Mandatory workflow for turning repeated task lessons, tool discoveries, failure fixes, user-specific procedures, and practical troubleshooting records into global Codex skills under ~/.codex/skills. Use after tasks that reveal reusable methods, recurring failure patterns, installation fixes, domain procedures, or user-provided operational notes. (file: r0/workflow-memory-skillsmith/SKILL.md)
- xlsx: Use this skill any time a spreadsheet file is the primary input or output. This means any task where the user wants to: open, read, edit, or fix an existing .xlsx, .xlsm, .csv, or .tsv file (e.g., adding columns, computing formulas, formatting, charting, cleaning messy data); create a new spreadsheet from scratch or from other data sources; or convert betw (file: r0/xlsx/SKILL.md)
### How to use skills
- Discovery: The list above is the skills available in this session (name + description + short path). Skill bodies live on disk at the listed paths after expanding the matching alias from `### Skill roots`.
- Trigger rules: If the user names a skill (with `$SkillName` or plain text) OR the task clearly matches a skill's description shown above, you must use that skill for that turn. Multiple mentions mean use them all. Do not carry skills across turns unless re-mentioned.
- Missing/blocked: If a named skill isn't in the list or the path can't be read, say so briefly and continue with the best fallback.
- How to use a skill (progressive disclosure):
  1) After deciding to use a skill, the main agent must expand the listed short `path` with the matching alias from `### Skill roots`, then open and read its `SKILL.md` completely before taking task actions. If a read is truncated or paginated, continue until EOF.
  2) When `SKILL.md` references relative paths (e.g., `scripts/foo.py`), resolve them relative to the directory containing that expanded `SKILL.md` first, and only consider other paths if needed.
  3) If `SKILL.md` points to extra folders such as `references/`, use its routing instructions to identify the files required for the task. The main agent must read each required instruction or reference file itself before acting on it. Do not delegate reading, summarizing, or interpreting skill instructions to a subagent. Subagents may still perform task work when the selected skill allows it.
  4) If `scripts/` exist, prefer running or patching them instead of retyping large code blocks.
  5) If `assets/` or templates exist, reuse them instead of recreating from scratch.
- Coordination and sequencing:
  - If multiple skills apply, choose the minimal set that covers the request and state the order you'll use them.
  - Announce which skill(s) you're using and why (one short line). If you skip an obvious skill, say why.
- Context hygiene:
  - Progressive disclosure applies to selecting relevant files, not partially reading a selected instruction file. Do not load unrelated references, scripts, or assets.
  - Avoid deep reference-chasing: prefer opening only files directly linked from `SKILL.md` unless you're blocked.
  - When variants exist (frameworks, providers, domains), pick only the relevant reference file(s) and note that choice.
- Safety and fallback: If a skill can't be applied cleanly (missing files, unclear instructions), state the issue, pick the next-best approach, and continue.
</skills_instructions>
<plugins_instructions>
## Plugins
A plugin is a local bundle of skills, MCP servers, and apps.
### How to use plugins
- Skill naming: If a plugin contributes skills, those skill entries are prefixed with `plugin_name:` in the Skills list.
- MCP naming: Plugin-provided MCP tools keep standard MCP identifiers such as `mcp__server__tool`; use tool provenance to tell which plugin they come from.
- Trigger rules: If the user explicitly names a plugin, prefer capabilities associated with that plugin for that turn.
- Relationship to capabilities: Plugins are not invoked directly. Use their underlying skills, MCP tools, and app tools to help solve the task.
- Relevance: Determine what a plugin can help with from explicit user mention or from the plugin-associated skills, MCP tools, and apps exposed elsewhere in this turn.
- Missing/blocked: If the user requests a plugin that does not have relevant callable capabilities for the task, say so briefly and continue with the best fallback.
</plugins_instructions>
## Memory

You have access to a memory folder with guidance from prior runs. It can save
time and help you stay consistent. Use it whenever it is likely to help.

Decision boundary: should you use memory for a new user query?

- Skip memory ONLY when the request is clearly self-contained and does not need
  workspace history, conventions, or prior decisions.
- Hard skip examples: current time/date, simple translation, simple sentence
  rewrite, one-line shell command, trivial formatting.
- Use memory by default when ANY of these are true:
  - the query mentions workspace/repo/module/path/files in MEMORY_SUMMARY below,
  - the user asks for prior context / consistency / previous decisions,
  - the task is ambiguous and could depend on earlier project choices,
  - the ask is a non-trivial and related to MEMORY_SUMMARY below.
- If unsure, do a quick memory pass.

Memory layout (general -> specific):

- C:\Users\t250c\.codex\memories/memory_summary.md (already provided below; do NOT open again)
- C:\Users\t250c\.codex\memories/MEMORY.md (searchable registry; primary file to query)
- C:\Users\t250c\.codex\memories/skills/<skill-name>/ (skill folder)
  - SKILL.md (entrypoint instructions)
  - scripts/ (optional helper scripts)
  - examples/ (optional example outputs)
  - templates/ (optional templates)
- C:\Users\t250c\.codex\memories/rollout_summaries/ (per-rollout recaps + evidence snippets)
  - The paths of these entries can be found in C:\Users\t250c\.codex\memories/MEMORY.md or C:\Users\t250c\.codex\memories/rollout_summaries/ as `rollout_path`
  - These files are append-only `jsonl`: `session_meta.payload.id` identifies the session, `turn_context` marks turn boundaries, `event_msg` is the lightweight status stream, and `response_item` contains actual messages, tool calls, and tool outputs.
  - For efficient lookup, prefer matching the filename suffix or `session_meta.payload.id`; avoid broad full-content scans unless needed.

Quick memory pass (when applicable):

1. Skim the MEMORY_SUMMARY below and extract task-relevant keywords.
2. Search C:\Users\t250c\.codex\memories/MEMORY.md using those keywords.
3. Only if MEMORY.md directly points to rollout summaries/skills, open the 1-2
   most relevant files under C:\Users\t250c\.codex\memories/rollout_summaries/ or
   C:\Users\t250c\.codex\memories/skills/.
4. If above are not clear and you need exact commands, error text, or precise evidence, search over `rollout_path` for more evidence.
5. If there are no relevant hits, stop memory lookup and continue normally.

Quick-pass budget:

- Keep memory lookup lightweight: ideally <= 4-6 search steps before main work.
- Avoid broad scans of all rollout summaries.

During execution: if you hit repeated errors, confusing behavior, or suspect
relevant prior context, redo the quick memory pass.

How to decide whether to verify memory:

- Consider both risk of drift and verification effort.
- If a fact is likely to drift and is cheap to verify, verify it before
  answering.
- If a fact is likely to drift but verification is expensive, slow, or
  disruptive, it is acceptable to answer from memory in an interactive turn,
  but you should say that it is memory-derived, note that it may be stale, and
  consider offering to refresh it live.
- If a fact is lower-drift and expensive to verify, it is usually fine to
  answer from memory directly.

When answering from memory without current verification:

- If you rely on memory for a fact that you did not verify in the current turn,
  say so briefly in the final answer.
- If that fact is plausibly drift-prone or comes from an older note, older
  snapshot, or prior run summary, say that it may be stale or outdated.
- If live verification was skipped and a refresh would be useful in the
  interactive context, consider offering to verify or refresh it live.
- Do not present unverified memory-derived facts as confirmed-current.
- Prefer a short refresh offer for interactive questions, especially about prior
  results, commands, timing, or older snapshots.

Memory citation requirements:

- If ANY relevant memory files were used: append exactly one
`<oai-mem-citation>` block as the VERY LAST content of the final reply.
  Normal responses should include the answer first, then append the
`<oai-mem-citation>` block at the end.
- Use this exact structure for programmatic parsing:
```
<oai-mem-citation>
<citation_entries>
MEMORY.md:234-236|note=[responsesapi citation extraction code pointer]
rollout_summaries/2026-02-17T21-23-02-LN3m-example.md:10-12|note=[weekly report format]
</citation_entries>
<rollout_ids>
019c6e27-e55b-73d1-87d8-4e01f1f75043
019c7714-3b77-74d1-9866-e1f484aae2ab
</rollout_ids>
</oai-mem-citation>
```
- `citation_entries` is for rendering:
  - one citation entry per line
  - format: `<file>:<line_start>-<line_end>|note=[<how memory was used>]`
  - use file paths relative to the memory base path (for example, `MEMORY.md`,
    `rollout_summaries/...`, `skills/...`)
  - only cite files actually used under the memory base path (do not cite
    workspace files as memory citations)
  - if you used `MEMORY.md` and then a rollout summary/skill file, cite both
  - list entries in order of importance (most important first)
  - `note` should be short, single-line, and use simple characters only (avoid
    unusual symbols, no newlines)
- `rollout_ids` is for us to track what previous rollouts you find useful:
  - include one rollout id per line
  - rollout ids should look like UUIDs (for example,
    `019c6e27-e55b-73d1-87d8-4e01f1f75043`)
  - include unique ids only; do not repeat ids
  - an empty `<rollout_ids>` section is allowed if no rollout ids are available
  - you can find rollout ids in rollout summary files and MEMORY.md
  - do not include file paths or notes in this section
  - For every `citation_entries`, try to find and cite the corresponding rollout id if possible
- Never include memory citations inside pull-request messages.
- Never cite blank lines; double-check ranges.

Updating memories:

You can update the memories **only** when explicitly asked by the user. This must always come from a direct request from the user.
- Write your update in C:\Users\t250c\.codex\memories/extensions/ad_hoc/notes/
- Each update must be one small file containing what you want to add/delete/update from the memories.
- The name of this file must be `<timestamp>-<short slug>.md`
- Do not try to edit the memory files yourself, only add one update note in C:\Users\t250c\.codex\memories/extensions/ad_hoc/notes/

========= MEMORY_SUMMARY BEGINS =========
v1

## User Profile
The user works with Codex on Windows for recurring industrial-engineering tasks: Qt upper-computer projects, CAN/protocol HTMLs, Keil/embedded firmware diagnosis, MCGS scripts, local app/tool troubleshooting, and Codex continuity/tooling repair. They expect agents to recover prior context from local memory, rollout artifacts, handoff bundles, or toolkit mirrors before asking them to restate it.

They prefer concrete, source-backed work: exact paths, exact files, exact values, runnable local steps, and deliverables they can open immediately. For embedded diagnosis they often want live-code tracing first and only want edits when they explicitly ask. When they say something should be remembered globally, they expect it to be persisted into durable memory or a reusable skill rather than left in chat.

They work across closely related Qt, algorithm PCB, and vehicle PCB trees, so routing by real `cwd`, real executable path, and real protocol/doc source matters. They also use Codex as an execution engine for local Windows tasks, not just for advice.

## User preferences
- When the user asks to recover memory or prior context, search local Codex memory/session artifacts first, then synced toolkit records, before answering from scratch.
- When the user gives a path or concrete local target, inspect the real path and continue the task instead of turning it into a clarification loop.
- For embedded source diagnosis, stay read-only and prove the exact branch/path in code unless the user explicitly asks for edits.
- When the user phrases a local task as `我把需求告诉你，你完成这个任务`, `自动给我搞`, or `本地有这个...你打开`, default to doing the real local work and verifying the result, not stopping at pseudocode or candidate paths.
- For MCGS or similar industrial scripting questions, answer in directly writable syntax and verify uncertain semantics from local help or reproducible behavior instead of relying on memory.
- When the user asks for a `故事` or `例子` to change team mindset, use a vivid, quotable analogy that directly rebuts `我们是按图纸来做的，没错` instead of abstract policy wording.
- When the user asks `发一个任务指令内容`, give a copy-pastable task block with file paths, variables, and verification steps instead of a high-level summary.
- When a task says `QT,算法PCB，整车PCB...全部内测`, keep the full three-way scope; do not shrink it to one pair or to document review only.
- When the user allows incomplete vehicle definitions during interop work, continue with code-level/build-level verification instead of blocking on perfect completeness.
- When the user asks to compare several source files like `siton / 老朱 / 自制`, answer source-by-source and print the exact blocks/values when they ask for them.
- For QDN vehicle PCB CAN edits, keep the old-engineering expanded style with literal CAN IDs directly searchable in Keil; do not hide `0x170`-`0x178` or receive parsing behind abstractions. [ad-hoc note]
- When the user says `QT上传服务器`, treat it as the QDN Qt upper-computer remote-update publish workflow under `http://kxdz.anyixt.com:9999/zkj/`, not the Keil monitor upload flow. [ad-hoc note]

## General Tips
- This environment is Windows with PowerShell; many useful fixes depend on real local paths under `C:\Users\t250c`, not just the current workspace.
- For Codex history/sidebar/workspace issues, inspect `state_5.sqlite`, `session_index.jsonl`, `.codex-global-state.json`, and relevant `rollout-*.jsonl` together.
- If Chinese text looks garbled, retry with UTF-8 or byte-level inspection before concluding the file is corrupted.
- The bundled Codex runtime Python at `C:\Users\t250c\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe` is the reliable fallback when system Python is missing or inconsistent.
- For current QDN Qt verification, default to `C:\Users\t250c\Documents\全电脑台车-CAN协议\qt-upper-computer-ui\build_audit_p201\QdnUpperComputerUi.exe` unless the user explicitly routes work to split `201` or `111` trees; older `build\QdnUpperComputerUi.exe` defaults can point at the wrong binary. [ad-hoc note]
- In current Qt/algorithm/vehicle interop, Qt owns local kinematics, the validated downlink batch is `0x50/0x51/0x71/0x75/0x7A`, and `0x179` should stay absent; grep for `TRIAD_INTEROP_STATIC_AUDIT_OK` when checking the last audited state.
- If the Qt upper-computer app `打不开`, check process lifecycle and same-path cleanup in `src/main.cpp` before blaming CAN, DLL, or protocol code; clear only the fixed-path `build_audit_p201\QdnUpperComputerUi.exe` process, because a pathless residual `QdnUpperComputerUi` can be misleading. [ad-hoc note]
- In `C:\Users\t250c\Documents\全电脑台车-CAN协议`, `201` means the navigation-only Qt line and `111` means the inverse-kinematics Qt line; both were split from `qt-upper-computer-ui` without build artifacts or `.git`, and `11…2375 tokens truncated…drilling direction and cooling-threshold logic: Auto_work_logic, Drill_Push_PWM, Drill_Back_PWM, bFan_DI, Cooling_Motor_DO, Vacuum_Fan_DO
  - desc: Use this for older 7-control firmware questions about reversed automatic drilling direction or derust-truck cooling/fan thresholds.

#### C:\Users\t250c\Documents\旭工 and E:\AI_划时代\旭工\干喷\程序\显示屏7-200

- MC_LCD water-pump remote/local branch diagnosis: MC_LCD - 7Control_V1.2, Water_setup_EN, Potentiometer_BUF2, A_R_F3, PWM2A, PIN_Binding, 0x152, 0xA23, 0xA50
  - desc: Use this when local water-pump control works but remote does not in the 旭工 `MC_LCD - 7Control_V1.2` firmware.

#### C:\Users\t250c\Documents\全电脑上位机

- Third-party Electron upper-computer package inspection and kinematics-display mapping: Electron, app.asar, poseOperation.js, renderer.js, StartPoseOperation, StartScada, initdata.json, Ouster, ZLGCAN
  - desc: Use this when the user points at the packed `上位机（siton）` release and asks what reusable motion/data/UI information it contains, or how the three-arm kinematics are actually shown.

- ChatGPT-plan plus Codex-execute workflow, and DevSpace fit check: ChatGPT 做方案, Codex 执行代码, Codex App, Codex CLI, DevSpace, Kubernetes, local repo
  - desc: Use this when the user asks whether an external tool fits their Windows Qt/Keil/CAN workflow, or wants discussion/planning split from local implementation/execution.

#### C:\Users\t250c\Documents\Codex temporary workdirs and Qt upper-computer repo

- `51 上位机QT` workspace disambiguation, split `201/111` routing, and `0x51` tracing: 51-qt, 51 上位机QT, qt-upper-computer-ui, 201, 111, 111_逆解.lnk, 0x51, 动态目标孔深, CanBusModel.cpp, CanToNetClient.cpp, MainWindow.cpp, McgsModbusTool, 116
  - desc: Use this when the user points at a fresh `51-qt` Codex folder or shorthand `201` / `111`; it separates the transient workdir, the original `qt-upper-computer-ui` repo, and the split navigation/inverse-kinematics lines, then traces `0x51` through source/docs.

#### C:\Users\t250c and local Codex/tooling

- Local skills, MCP config, sidebar repair, and plugin install: keil5-embedded-c, workflow-memory-skillsmith, config.toml, mcp_servers, session_index.jsonl, .codex-global-state.json, cowart
  - desc: Use this for expanding the local skill stack, fixing `config.toml` MCP tables, repairing missing sidebar history, or validating local plugin installs.

- Local auth-state and Jet `.ldb` lock-file inspection: auth.json, .codex, cmdkey, can_to_net, Standard Jet DB, .ldb, MCP, file lock
  - desc: Use this when the user asks whether Codex login is saved locally, or points at a Windows `.ldb` file and wants a direct identification from on-machine evidence.

- Codex continuity sync, cross-computer merge, and chat export: codex-personal-toolkit, session_index.jsonl, chat-records, work-states, external-record-continuity, work-continuity-sync, multi-computer-toolkit-merge
  - desc: Use this for sidebar-chat export, USB/GitHub handoff ingestion, full work-state upload, and merge-not-overwrite behavior across Lenovo and Dell machines.

- visual-plan install and short slash-command aliases: visual-plan, /plan, /vp, C:\Users\t250c\.codex\commands, Agent-Native Plan, npx -y
  - desc: Use this when the user wants the local visual planning skill installed, revalidated, or made easier to invoke with shorter command aliases on this machine.

#### E:\工作 and related temporary Codex workdirs

- Weekly automation and workspace status reporting: E:\工作, weekly workspace scan, cron, holding pattern, no new activity
  - desc: Use this when the user wants another weekly status update based on workspace activity, or wants the automation prompt shape reused without restating the full background.

- Research drawings versus现场 confirmation story analogies: research drawings, 现场沟通, production document, 我们是按图纸来做的, 形象一点的例子, 研究院图纸只是设计依据
  - desc: Use this when the user wants internal-training wording or a meeting story explaining why research-institute drawings must be reconciled with现场 before they become production files.

#### C:\Users\t250c and local desktop troubleshooting

- AweSun/向日葵 control-side failure diagnosis and reinstall: AweSun, Sunlogin, shell.log, fastcode was forbidden, sys_config.ini, AweSun.exe --mod=uninstall
  - desc: Use this when others can still control this PC but this PC cannot control other machines, and the user wants diagnosis plus reinstall without losing controlled-side identity/config.

#### C:\Users\t250c and Desktop manuals

- CR0020 manual indexing and `Q11_MODE` screenshot interpretation: 7391027_ZHCN编程手册CR0020.pdf, ClassicController, Q11_MODE, %QX0.01, OUT_DIGITAL_H, OUT_CURRENT, OUT_DIAGNOSTIC, OUT_OVERLOAD_PROTECTION, 0xC5
  - desc: Use this when the user wants a local CR0020 manual read, page routing, or exact mode-byte decoding from a screenshot.
========= MEMORY_SUMMARY ENDS =========

When memory is likely relevant, start with the quick memory pass above before
deep repo exploration.
```

### user 2026-07-06T01:30:57.226Z

```text
# AGENTS.md instructions

<INSTRUCTIONS>
#1. Think Before Coding  
**Don't assume. Don't hide confusion. Surface tradeoffs.**  
Before implementing:  
- State your assumptions explicitly. If uncertain, ask.  
- If multiple interpretations exist, present them—don't pick silently.  
- If a simpler approach exists, say so. Push back when warranted.  

#2. Simplicity First  
***Minimum code that solves the problem. Nothing speculative.***  
- No features beyond what was asked.  
- No abstractions for single-use code.  
- No "flexibility" for unrequested scenarios.  
- Rewrite 200-line solutions down to 50 if possible.
</INSTRUCTIONS>
<environment_context>
  <cwd>E:\工作</cwd>
  <shell>powershell</shell>
  <current_date>2026-07-06</current_date>
  <timezone>Asia/Shanghai</timezone>
  <filesystem><workspace_roots><root>E:\工作</root></workspace_roots><permission_profile type="disabled"><file_system type="unrestricted" /></permission_profile></filesystem>
</environment_context>
```

### user 2026-07-06T01:30:57.250Z

```text
Automation: 每周工作状态更新
Automation ID: automation
Automation memory: $CODEX_HOME/automations/automation/memory.md
Last run: 2026-06-29T01:00:33.581Z (1782694833581)

Review recent work activity in the workspace and draft a brief weekly status update. Summarize completed work, current in-progress items, blockers or risks, and suggested next steps. Keep it concise and ready to send or adapt.
```

### assistant 2026-07-06T01:31:03.960Z

```text
Reviewing `E:\工作` for activity since the last run on 2026-06-29 and checking the automation memory first so this update stays consistent.
```

### assistant 2026-07-06T01:31:13.806Z

```text
The workspace itself has almost no new top-level file activity after the last run, so I’m checking git history and deeper file timestamps to separate real work from metadata touches.
```

### assistant 2026-07-06T01:31:24.501Z

```text
No file writes showed up after 2026-06-29. I’m checking whether the top-level `.git` is incomplete metadata and whether any nested project folders changed in ways the file scan missed.
```

### assistant 2026-07-06T01:31:36.804Z

```text
The scan is consistent with the last two runs: no substantive workspace changes this week, only empty metadata directories created on 2026-06-29. I’m writing a short run note to the automation memory, then I’ll return the weekly status draft.
```

### assistant 2026-07-06T01:31:51.701Z

```text
Weekly status update for `E:\工作`:

Completed work:
The workspace still reflects the earlier setup/documentation baseline only: Qt helper scripts, the `QtSmokeTest` build output, the `QtOpenCVTemplate` scaffold, and `全电脑CAN协议记录.html` as the latest substantive deliverable.

In progress:
No new implementation or validation activity was detected since the last run on 2026-06-29. The only newer changes are empty metadata folders (`.agents`, `.codex`, `.git`) created on 2026-06-29, which do not show actual project progress.

Blockers / risks:
Status tracking is limited because this workspace has no usable git history, and `QtOpenCVTemplate` still has no evidence of a verified OpenCV build/run. That means work is either paused or happening outside this workspace.

Suggested next steps:
Either resume from this baseline by building and running `QtOpenCVTemplate`, or explicitly mark/archive this workspace as dormant so future weekly scans do not keep reporting the same no-change state.

::inbox-item{title="Workspace dormant status confirmed" summary="No new work detected; resume build or archive workspace"}
```
