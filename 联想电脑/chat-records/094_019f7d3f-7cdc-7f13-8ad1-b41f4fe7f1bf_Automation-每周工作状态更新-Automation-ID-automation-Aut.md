# Automation: 每周工作状态更新 Automation ID: automation Automation memory: $CODEX_HOME/au

- Session id: `019f7d3f-7cdc-7f13-8ad1-b41f4fe7f1bf`
- Source cwd: `E:\工作`
- Model provider: `openai`
- Exported at: `2026-07-21T23:40:20`
- Local source: `C:\Users\t250c\.codex\sessions\2026\07\20\rollout-2026-07-20T09-59-00-019f7d3f-7cdc-7f13-8ad1-b41f4fe7f1bf.jsonl`

## Transcript

### developer 2026-07-20T01:59:05.154Z

```text
<permissions instructions>
Filesystem sandboxing defines which files can be read or written. `sandbox_mode` is `workspace-write`: The sandbox permits reading files, and editing files in `cwd` and `writable_roots`. Editing files in other directories requires approval. Network access is restricted.
# Escalation Requests

Commands are run outside the sandbox if they are approved by the user, or match an existing rule that allows it to run unrestricted. The command string is split into independent command segments at shell control operators, including but not limited to:

- Pipes: |
- Logical operators: &&, ||
- Command separators: ;
- Subshell boundaries: (...), $(...)

Each resulting segment is evaluated independently for sandbox restrictions and approval requirements.

Example:

git pull | tee output.txt

This is treated as two command segments:

["git", "pull"]

["tee", "output.txt"]

Commands that use more advanced shell features like redirection (>, >>, <), substitutions ($(...), ...), environment variables (FOO=bar), or wildcard patterns (*, ?) will not be evaluated against rules, to limit the scope of what an approved rule allows.

## How to request escalation

IMPORTANT: To request approval to execute a command that will require escalated privileges:

- Provide the `sandbox_permissions` parameter with the value `"require_escalated"`
- Include a short question asking the user if they want to allow the action in `justification` parameter. e.g. "Do you want to download and install dependencies for this project?"
- Optionally suggest a `prefix_rule` - this will be shown to the user with an option to persist the rule approval for future sessions.

If you run a command that is important to solving the user's query, but it fails because of sandboxing or with a likely sandbox-related network error (for example DNS/host resolution, registry/index access, or dependency download failure), rerun the command with "require_escalated". ALWAYS proceed to use the `justification` parameter - do not message the user before requesting approval for the command.

## When to request escalation

While commands are running inside the sandbox, here are some scenarios that will require escalation outside the sandbox:

- You need to run a command that writes to a directory that requires it (e.g. running tests that write to /var)
- You need to run a GUI app (e.g., open/xdg-open/osascript) to open browsers or files.
- If you run a command that is important to solving the user's query, but it fails because of sandboxing or with a likely sandbox-related network error (for example DNS/host resolution, registry/index access, or dependency download failure), rerun the command with `require_escalated`. ALWAYS proceed to use the `sandbox_permissions` and `justification` parameters. do not message the user before requesting approval for the command.
- You are about to take a potentially destructive action such as an `rm` or `git reset` that the user did not explicitly ask for.
- Be judicious with escalating, but if completing the user's request requires it, you should do so - don't try and circumvent approvals by using other tools.

## prefix_rule guidance

When choosing a `prefix_rule`, request one that will allow you to fulfill similar requests from the user in the future without re-requesting escalation. It should be categorical and reasonably scoped to similar capabilities. You should rarely pass the entire command into `prefix_rule`.

### Banned prefix_rules 
Avoid requesting overly broad prefixes that the user would be ill-advised to approve. For example, do not request ["python3"], ["python", "-"], or other similar prefixes that would allow arbitrary scripting.
NEVER provide a prefix_rule argument for destructive commands like rm.
NEVER provide a prefix_rule if your command uses a heredoc or herestring. 

### Examples
Good examples of prefixes:
- ["npm", "run", "dev"]
- ["gh", "pr", "check"]
- ["cargo", "test"]


## Approved command prefixes
The following prefix rules have already been approved: - ["C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe", "-Command", "try { $p='E:\\IPMsg\\gbk-garbled-comments\\.codex_write_test'; [System.IO.File]::WriteAllText($p,'test',[System.Text.Encoding]::ASCII); [System.IO.File]::Delete($p); 'write-ok' } catch { 'write-failed: ' + $_.Exception.Message }"]
- ["C:\\Program Files\\PowerShell\\7\\pwsh.exe", "-Command", "Get-ChildItem -Path \"C:\\Users\\t250c\\Documents\\全电脑台车-CAN协议\\qt-upper-computer-ui\\src\" -Filter \"MainWindow.cpp.bak_*\" | Remove-Item -Force; $src=\"C:\\Users\\t250c\\Documents\\全电脑台车-CAN协议\\qt-upper-computer-ui\\src\\MainWindow.cpp\"; $ts=Get-Date -Format \"yyyyMMdd_HHmmss\"; Copy-Item $src \"$src.bak_$ts\""]
- ["C:\\Program Files\\PowerShell\\7\\pwsh.exe", "-Command", "$file='src\\MainWindow.cpp'; $dir=Split-Path -Parent $file; $name=Split-Path -Leaf $file; Get-ChildItem -LiteralPath $dir -Filter \"$name.bak_*\" -File -ErrorAction SilentlyContinue | Remove-Item -Force; $stamp=Get-Date -Format 'yyyyMMdd_HHmmss'; $bak=\"$file.bak_$stamp\"; Copy-Item -LiteralPath $file -Destination $bak -Force; Write-Output $bak"]
- ["C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe", "-Command", "$src='C:\\Users\\t250c\\Documents\\Codex\\2026-05-23\\new-chat\\gbk-garbled-comments_SKILL.fixed.md'; $dst='E:\\IPMsg\\gbk-garbled-comments\\SKILL.md'; $bak=\"E:\\IPMsg\\gbk-garbled-comments\\SKILL.md.bak_$(Get-Date -Format yyyyMMdd_HHmmss)\"; Copy-Item -LiteralPath $dst -Destination $bak; Copy-Item -LiteralPath $src -Destination $dst -Force; \"backup=$bak\""]
- ["C:\\Program Files\\PowerShell\\7\\pwsh.exe", "-Command", "$file = 'C:\\Users\\t250c\\Documents\\全电脑台车-CAN协议\\external\\can-variable-monitor\\McgsModbusTool\\MainForm.cs'; Get-ChildItem -LiteralPath (Split-Path $file) -Filter 'MainForm.cs.bak_*' | Remove-Item -Force; $stamp = Get-Date -Format 'yyyyMMdd_HHmmss'; Copy-Item -LiteralPath $file -Destination \"$file.bak_$stamp\"; Get-Item \"$file.bak_$stamp\" | Select-Object FullName,Length"]
- ["C:\\Program Files\\PowerShell\\7\\pwsh.exe", "-Command", "$dir='C:\\Users\\t250c\\Documents\\全电脑台车-CAN协议\\pcb-can-simulator'; $name='FrameState.cs'; $file=Join-Path $dir $name; Get-ChildItem -LiteralPath $dir -Filter \"$name.bak_*\" -File -ErrorAction SilentlyContinue | Remove-Item -Force; $stamp=Get-Date -Format 'yyyyMMdd_HHmmss'; Copy-Item -LiteralPath $file -Destination \"$file.bak_$stamp\" -Force; Write-Output \"$file.bak_$stamp\""]
- ["C:\\Program Files\\PowerShell\\7\\pwsh.exe", "-Command", "$file = 'docs\\CAN协议_上位机覆盖梳理_20260620_实现进度.xlsx'; $dir = Split-Path -Parent $file; $name = Split-Path -Leaf $file; Get-ChildItem -LiteralPath $dir -Filter \"$name.bak_*\" -File -ErrorAction SilentlyContinue | Remove-Item -Force; $stamp = Get-Date -Format 'yyyyMMdd_HHmmss'; Copy-Item -LiteralPath $file -Destination \"$file.bak_$stamp\" -Force; Write-Output \"$file.bak_$stamp\""]
- ["C:\\Program Files\\PowerShell\\7\\pwsh.exe", "-Command", "$file='C:\\Users\\t250c\\Documents\\全电脑上位机\\.codex\\PROJECT_MEMORY.md'; $dir=Split-Path -Parent $file; $name=Split-Path -Leaf $file; Get-ChildItem -LiteralPath $dir -Filter \"$name.bak_*\" -File -ErrorAction SilentlyContinue | Remove-Item -Force; $stamp=Get-Date -Format 'yyyyMMdd_HHmmss'; $bak=\"$file.bak_$stamp\"; Copy-Item -LiteralPath $file -Destination $bak -Force; Write-Output \"backup=$bak\""]
- ["C:\\Program Files\\PowerShell\\7\\pwsh.exe", "-Command", "$file='C:\\Users\\t250c\\Documents\\全电脑台车-CAN协议\\pcb-can-simulator\\MainForm.cs'; $dir=Split-Path -Parent $file; $name=Split-Path -Leaf $file; Get-ChildItem -LiteralPath $dir -Filter \"$name.bak_*\" -File -ErrorAction SilentlyContinue | Remove-Item -Force; $stamp=Get-Date -Format 'yyyyMMdd_HHmmss'; $bak=\"$file.bak_$stamp\"; Copy-Item -LiteralPath $file -Destination $bak -Force; Write-Output $bak"]
- ["C:\\Program Files\\PowerShell\\7\\pwsh.exe", "-Command", "$dir='C:\\Users\\t250c\\Documents\\全电脑台车-CAN协议\\pcb-can-simulator'; $stamp=Get-Date -Format 'yyyyMMdd_HHmmss'; foreach($name in @('MainForm.cs','README.md')){ $file=Join-Path $dir $name; Get-ChildItem -LiteralPath $dir -Filter \"$name.bak_*\" -File -ErrorAction SilentlyContinue | Remove-Item -Force; Copy-Item -LiteralPath $file -Destination \"$file.bak_$stamp\" -Force; Write-Output \"$file.bak_$stamp\" }"]
- ["C:\\Program Files\\PowerShell\\7\\pwsh.exe", "-Command", "$file='C:\\Users\\t250c\\Documents\\全电脑台车-CAN协议\\pcb-can-simulator\\ModbusTcpServer.cs'; $dir=Split-Path -Parent $file; $name=Split-Path -Leaf $file; Get-ChildItem -LiteralPath $dir -Filter \"$name.bak_*\" -File -ErrorAction SilentlyContinue | Remove-Item -Force; $stamp=Get-Date -Format 'yyyyMMdd_HHmmss'; $bak=\"$file.bak_$stamp\"; Copy-Item -LiteralPath $file -Destination $bak -Force; Write-Output $bak"]
- ["C:\\Program Files\\PowerShell\\7\\pwsh.exe", "-Command", "$file='C:\\Users\\t250c\\Documents\\全电脑台车-CAN协议\\pcb-can-simulator\\SimulatorSelfTest.cs'; $dir=Split-Path -Parent $file; $name=Split-Path -Leaf $file; Get-ChildItem -LiteralPath $dir -F...
[Some commands were truncated]

`approvals_reviewer` is `auto_review`: Sandbox escalations with require_escalated will be reviewed for compliance with the policy. If a rejection happens, you should proceed only with a materially safer alternative, or inform the user of the risk and send a final message to ask for approval.
 The writable roots are `C:\Users\t250c\.codex\automations\automation`, `E:\工作`.
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
- Treat the terms "task", "thread", "chat", and "conversation" as synonyms when they clearly refer to Codex. Tool names use the term "thread" and Codex uses "task" in the UI. When providing user-facing responses, use "task".
- When the user asks to create, fork, inspect, continue, hand off, pin, archive, rename, or otherwise manage Codex threads, search for the relevant thread tool first: `create_thread`, `fork_thread`, `list_threads`, `read_thread`, `wait_threads`, `send_message_to_thread`, `handoff_thread`, `set_thread_pinned`, `set_thread_archived`, or `set_thread_title`.
- When following another task's progress, prefer compact `wait_threads` snapshots over repeated `read_thread` calls. Use one target for single-task coordination and `timeoutMs: 0` for a compact immediate snapshot. `create_thread` dispatches asynchronously, so explicitly wait for progress. Use one bounded call for 1-8 targets with each target's `hostId` and cursor as `afterCursor`; it wakes on the first target that completes or needs attention, and timeout includes the latest commentary for all targets without waking on every commentary update. An up-to-date cursor suppresses already-delivered final text. Separate waits from one task may run serially. Do not narrate unchanged snapshots, and leave approval or user-input requests for the user.
- Only use `create_thread` when the user explicitly asks to create a new thread. Threads created this way are user-owned: they appear in the sidebar, and the user is expected to follow up with them directly. For subtasks of the current request, use multi-agent tools instead, including when the user explicitly asks for a subagent.
- After a successful `create_thread` call, emit `::created-thread{threadId="..."}` for a created thread or `::created-thread{clientThreadId="..."}` for queued worktree setup on its own line in your final response.

### Inline Code Comments
- Use the ::code-comment{...} directive when you need to attach feedback directly to specific code lines.
- Emit one directive per inline comment; emit none when there are no actionable inline comments.
- Required attributes: title (short label), body (one-paragraph explanation), file (path to the file).
- Optional attributes: start, end (1-based line numbers), priority (0-3).
- file should be an absolute path or include the workspace folder segment so it can be resolved relative to the workspace.
- Keep line ranges tight; end defaults to start.
- Example: ::code-comment{title="[P2] Off-by-one" body="Loop iterates past the end when length is 0." file="/path/to/foo.ts" start=10 end=11 priority=2}
</app-context>
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

Industrial embedded/Qt/CAN and machine-vision practitioner working mainly on Windows local projects. Recurring work includes Keil/LPC firmware, HMI/VGUS displays, CAN/CANopen, Qt upper-computer applications, calibration/geometry, release workflows, and cross-computer Codex continuity. Values concrete source/path/frame evidence, minimally scoped edits, backups, and verification that matches the real task (build, artifact/hash, self-test, field log, or UI evidence).

## User preferences

- Use the existing compatible version; “能满足即可！！” means do not downgrade just to match an older requirement.
- For local installs, provide a concrete user-run check; a single double-click `.bat` path is preferred when practical.
- When a symptom is tied to an HMI page or CAN frame, map the exact page/address/bytes first; do not answer from a nearby variable or generic theory.
- For protocol questions, give exact IDs, byte order, source/doc references, and state remaining uncertainty rather than claiming full coverage.
- Keep engineering fixes minimal and scoped; state exactly what changed, make backups before edits, then verify the resulting artifact.
- Do not substitute timeout widening, AUTO behavior, or a one-machine workaround for an identified structural cause.
- For cross-computer continuity, merge rather than overwrite; standalone “上传” means full work-state sync unless narrowed.

## General Tips

- In legacy Keil work, confirm the active startup file and map before inferring stack/RAM facts; `Maximum Stack Usage` against `Stack_Size` is high-priority evidence.
- Treat a compiled/local self-test as distinct from hardware field proof. Keep pending hardware retests explicit.
- For historical hardware/project questions, search current sources plus `codex-personal-toolkit` chat records; narrow file types to avoid venv noise.
- For config/package publication, verify by read-back and SHA256; immediate upload errors such as `curl: (52)` can be inconclusive.

## What's in Memory

### C:\Users\t250c\Documents\Codex\2026-07-17\e-ai-g-7-17

#### 2026-07-17

- Page 16 stack overflow and E10/E11 signed VGUS writes: Page16, gPwm_BD, Maximum Stack Usage, Stack_Size, E10, E11, LCD_WR_Data2B
  - desc: Start here for Page 16 jumps, signed negative angles, or same-BIN/different-controller startup cases.
  - learnings: Map page/address first; 1672+Unknown bytes versus 0x300 stack supported corruption, while explicit signed conversion produced two's-complement E10/E11 frames.

### E:\AI_划时代\下载器\AI_boot

#### 2026-07-17

- AI_boot handshake, SYS send result, and IAP stop path: 0x702, 0x703, WriteCanMsg, lastSentCount, 0x12, 0x17, sbl_iap.c
  - desc: Source-backed routing for downloader phases and the partial SYS patch.
  - learnings: Wait is 0x702; new hardware logs are required; this bootloader's IAP `while(1)` is stronger evidence than stack as the missing-ACK cause.

### C:\Users\t250c\Documents\Codex\Vision_AI_Project

#### 2026-07-17

- Windows Vision_AI_Project and MV-CH120-10GM: Python 3.12.10, torch 2.13.0+cu126, run_vision_ai.bat, src/vision_ai, MV-CH120-10GM
  - desc: Venv bootstrap, formal YOLO layout, benchmark, and learning sequence.
  - learnings: Use the explicit venv/BAT; RTX 4060 8 GB validated YOLO11n but physical acquisition/lighting/latency still determine industrial readiness.

### E:\AI_划时代\视觉\QtCameraCalibration_codex\QtCameraCalibration

#### 2026-07-17

- K/D + AprilTag/total-station calibration: CalibrationWorker.cpp, TagPoseWorker.cpp, solvePnP, calibration.yml, TagCenter_world_calc
  - desc: Pipeline, operator HTML, and camera-history routing.
  - learnings: Fresh camera/YAML must match frame size; held-out Tag validation does not replace direct world-coordinate error reporting.

### Older Memory Topics

#### E:\AI_划时代 embedded / CAN / HMI projects

- XTGT3313X EEPROM and cooling review: Cooling_Y40_Logic, EX_BDINFO_STADDR2, AT24C256, App_Bus.h
  - desc: `cwd=E:\AI_划时代\x鑫通\XTGT3313X\MC_LCD(XTGT3313X) - 7C_V0.2`; minimal EEPROM-overlap repair and long-run counter-risk framing.
- MC_LCD geometry and helpers: V1_参考点, O点, Q4, Arm200A_Store, XY/XZ/YZ
  - desc: Arm200A helper-angle update and true-O/fit-calibration continuity.
- CanVariableMonitor update release: AppUpdateService, PublishUnified.ps1, V1.564, UTF8Encoding(false), can_monitor_latest.zip
  - desc: `cwd=C:\Users\t250c\Documents\Keil监控项目\can-variable-monitor-github-main`; multi-source detection and dual-server read-back.
- MCGS heartbeat and wording: can_to_net, AnswerAlm, 170~179, MCGS
  - desc: `cwd=C:\Users\t250c\Desktop\can_to_net`; script syntax and operator-facing explanations.

#### C:\Users\t250c\Documents\全电脑台车-CAN协议 / 全电脑上位机

- Qt CAN coverage and encoder CANopen: CanBusModel.cpp, 0x154, 0x181~0x1A9, 0x1800:05, TPDO1
  - desc: Protocol audit limits, raw sensor mapping, and little-endian SDO parsing.
- QDN handoff, CAN-NET, and app workflows: qt-upper-computer-ui, build_audit_p201, QdnUpperComputerUi.exe, H点, A2/A4
  - desc: Real repo routing, H-point factory fixture, controller/page behavior, and release/update skill `skills/qdn-qt-upload-server/SKILL.md`.
- Third-party Qt/Electron and prototype fallback: app.asar, poseOperation.js, jumbo-drill-designer-master, Vite
  - desc: Reusable inspection clues plus fallback when native Qt build/run is unavailable.

#### C:\Users\t250c local tooling and continuity

- Codex skills/MCP/history/plugins: config.toml, mcp_servers, session_index.jsonl, cowart
  - desc: Local skill creation/install, TOML duplicate-table repair, sidebar recovery, plugin validation.
- Cross-computer archive and upload: codex-personal-toolkit, work-states, session_index.jsonl, merge not overwrite
  - desc: Lenovo/Dell sync, USB/GitHub ingestion, and full-state continuity skills.
- Visual-plan aliases and Windows diagnostics: visual-plan, /plan, /vp, auth.json, .ldb, AweSun
  - desc: Local command discoverability, auth/Jet lock-file inspection, and remote-control reinstall evidence.

#### Other durable task families

- Reverse engineering and release hygiene: Ghidra, force-enable, Qt release, strings, protocol constants
  - desc: User-owned BIN minimal patching and realistic Qt release exposure assessment.
- Weekly status and communication: E:\工作, dormant gitless workspace, research drawings, 现场沟通
  - desc: Concise status reports and source-faithful internal-training analogies.
========= MEMORY_SUMMARY ENDS =========

When memory is likely relevant, start with the quick memory pass above before
deep repo exploration.

<apps_instructions>
## Apps (Connectors)
Apps (Connectors) can be explicitly triggered in user messages in the format `[$app-name](app://{connector_id})`. Apps can also be implicitly triggered as long as the context suggests usage of available apps.
An app is equivalent to a set of MCP tools within the `codex_apps` MCP.
An installed app's MCP tools are either provided to you already, or can be lazy-loaded through the `tool_search` tool. If `tool_search` is available, the apps that are searchable by `tools_search` will be listed by it.
Do not additionally call list_mcp_resources or list_mcp_resource_templates for apps.
</apps_instructions>
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
<skills_instructions>
## Skills
A skill is a set of local instructions to follow that is stored in a `SKILL.md` file. Below is the list of skills that can be used. Each entry includes a name, description, and a short path that can be expanded into an absolute path using the skill roots table.
### Skill roots
- `r0` = `C:/Users/t250c/.codex/skills`
- `r1` = `C:/Users/t250c/.agents/skills`
- `r2` = `C:/Users/t250c/.codex/skills/.system`
- `r3` = `C:/Users/t250c/.codex/plugins/cache/openai-bundled`
- `r4` = `C:/Users/t250c/.codex/plugins/cache/openai-bundled/sites/0.1.30/skills`
- `r5` = `C:/Users/t250c/.codex/plugins/cache/openai-curated-remote/github/0.1.8-2841cf9749ae/skills`
- `r6` = `C:/Users/t250c/.codex/plugins/cache/openai-curated-remote/openai-developers/1.2.3/skills`
- `r7` = `C:/Users/t250c/.codex/plugins/cache/openai-primary-runtime`
- `r8` = `C:/Users/t250c/.codex/plugins/cache/openai-primary-runtime/spreadsheets/26.715.12143/skills`
- `r9` = `C:/Users/t250c/.codex/plugins/cache/personal/cowart/0.1.3/skills`
### Available skills
- imagegen: Generate or edit raster images when the task benefits from AI-created bitmap visuals such as photos, illustrations, textur (file: r2/imagegen/SKILL.md)
- openai-docs: Use when the user asks how to build with OpenAI products or APIs, asks about Codex itself or choosing Codex surfaces, needs  (file: r2/openai-docs/SKILL.md)
- plugin-creator: Create and scaffold plugin directories for Codex with a required `.codex-plugin/plugin.json`, optional plugin folders/file (file: r2/plugin-creator/SKILL.md)
- skill-creator: Guide for creating effective skills. This skill should be used when users want to create a new skill (or update an existing  (file: r2/skill-creator/SKILL.md)
- skill-installer: Install Codex skills into $CODEX_HOME/skills from a curated list or a GitHub repo path. Use when a user asks to list install (file: r2/skill-installer/SKILL.md)
- arm-cortex-expert: ARM Cortex-M embedded development guidance for startup, vector table, NVIC, SysTick, low power, DMA, cache coherency, MPU, f (file: r0/arm-cortex-expert/SKILL.md)
- backup-and-edit: 修改文件前自动备份，并删除上一次备份（只保留最新一份）。适用于所有需要修改的文件，特别是C/H源码文件。 (file: r0/backup-and-edit/SKILL.md)
- browser:control-in-app-browser: Control the in-app Browser for opening, navigating, inspecting visible or interactive page state, clicking, typing, screen (file: r3/browser/26.715.31251/skills/control-in-app-browser/SKILL.md)
- cc-switch-provider-routing: Use when working with CC Switch providers backed by third-party OpenAI-compatible or Claude-compatible gateways, especiall (file: r0/cc-switch-provider-routing/SKILL.md)
- chat-transcript-uploader: Export local Codex chat/session records to timestamped documents and upload them to the user's GitHub toolkit repository.  (file: r0/chat-transcript-uploader/SKILL.md)
- computer-use:computer-use: Control Windows apps from ChatGPT (file: r3/computer-use/26.715.31251/skills/computer-use/SKILL.md)
- cowart:cowart-image-edit: Generate new AI images from user-supplied Cowart annotation screenshots. Use when the user provides one or more screensho (file: r9/cowart-image-edit/SKILL.md)
- cowart:cowart-image-gen: Generate a final AI bitmap for the Cowart canvas, including any requested in-image text by default. Use when the user asks  (file: r9/cowart-image-gen/SKILL.md)
- cowart:cowart-open-canvas: Open the Cowart local web service, a tldraw-powered infinite canvas. Use when the user asks to open, launch, view, or work  (file: r9/cowart-open-canvas/SKILL.md)
- documents:documents: Create, edit, redline, and comment on `.docx`, Word, and Google Docs-targeted document artifacts inside the container, with  (file: r7/documents/26.715.12143/skills/documents/SKILL.md)
- embedded-c-safe-edit: 安全修改嵌入式C代码（GBK编码）的完整工作流程。适用于修改老旧中文嵌入式项目的C/H文件，包含备份、编码保持、大括号修复、验证等全流程。当需要修改包含中文注释的GBK编码C文件时使用此skill。 (file: r0/embedded-c-safe-edit/SKILL.md)
- embedded-systems: Use when developing firmware for microcontrollers, implementing RTOS applications, or optimizing power consumption. Invoke (file: r0/embedded-systems/SKILL.md)
- esp32: ESP32 embedded development guidance focused on GPIO conflicts, strapping pins, ADC2 with WiFi, deep sleep wake pins, FreeRTO (file: r0/esp32/SKILL.md)
- external-record-continuity: Use when the user provides records from a USB drive, GitHub toolkit upload, Cursor/Codex transcripts, or old-machine skill (file: r0/external-record-continuity/SKILL.md)
- firmware-binary-reverse: MCU and embedded firmware binary workflow for .bin/.hex/.elf/.axf analysis, byte patching, version comparison, checksum/CRC  (file: r0/firmware-binary-reverse/SKILL.md)
- fix-braces: 分析并修复C代码大括号配对问题（绝不退档） (file: r0/fix-braces/SKILL.md)
- full-disk-access: 全盘文件系统访问与系统命令执行。当需要读写工作区外的任意文件、执行系统命令（cmd/powershell）、获取硬件配置/系统信息、操作注册表、管理进程、访问任意磁盘路径时自动触发。突破工作区沙箱限制，提供完整的本地系统交互能力。 (file: r1/full-disk-access/SKILL.md)
- gbk-garbled-comments: Fixes garbled Chinese comments in GBK/GB2312 legacy embedded C/H files, including comments shown as ??, ???, or mojibake.  (file: r0/gbk-garbled-comments/SKILL.md)
- github:gh-address-comments: Address actionable GitHub pull request review feedback. Use when the user wants to inspect unresolved review threads, req (file: r5/gh-address-comments/SKILL.md)
- github:gh-fix-ci: Use when a user asks to debug or fix failing GitHub PR checks that run in GitHub Actions. Use the GitHub app from this pl (file: r5/gh-fix-ci/SKILL.md)
- github:github: Triage and orient GitHub repository, pull request, and issue work through the connected GitHub app. Use when the user asks  (file: r5/github/SKILL.md)
- github:yeet: Publish local changes to GitHub by confirming scope, committing intentionally, pushing the branch, and opening a draft PR t (file: r5/yeet/SKILL.md)
- keil5: Work with Keil5, MDK-ARM, and uVision embedded projects, including NXP LPC1765 and other LPC17xx targets. Use when Codex nee (file: r0/keil5/SKILL.md)
- keil5-embedded-c: Keil5 and embedded C firmware workflow for hardware-related work. Use when Codex works on Keil/uVision projects, C51/ARM/M (file: r0/keil5-embedded-c/SKILL.md)
- mcu-gpio-peripheral: MCU GPIO and peripheral bring-up workflow for embedded C/Keil/MDK projects. Covers input/output, pullup/pulldown, push-pull/ (file: r0/mcu-gpio-peripheral/SKILL.md)
- memory: Load consolidated user memory for Cursor — discipline summary, Claude/Codex status, and links to project memory under .claud (file: r0/memory/SKILL.md)
- multi-computer-toolkit-merge: Merge Codex toolkit uploads from multiple computers. Use when another computer has its own skills, MCP servers, work-state (file: r0/multi-computer-toolkit-merge/SKILL.md)
- openai-developers:agents-sdk: Build, run, deploy, and evaluate OpenAI Agents SDK apps from Codex. Use when the user asks to create or adapt an Agents SDK  (file: r6/agents-sdk/SKILL.md)
- openai-developers:build-chatgpt-app: Build, scaffold, refactor, and troubleshoot ChatGPT Apps SDK applications that combine an MCP server and widget UI. Use wh (file: r6/build-chatgpt-app/SKILL.md)
- openai-developers:chatgpt-app-submission: Inspect a ChatGPT Apps MCP server codebase and generate chatgpt-app-submission.json with app info suggestions, tool hint jus (file: r6/chatgpt-app-submission/SKILL.md)
- openai-developers:openai-api-troubleshooting: Use when an OpenAI API request fails and Codex needs to classify the likely cause, explain the next step, and route to the r (file: r6/openai-api-troubleshooting/SKILL.md)
- openai-developers:openai-platform-api-key=***REDACTED*** when Codex is asked to build, run, test, debug, or configure an OpenAI-backed or provider-unspecified AI app, UI, scri (file: r6/openai-platform-api-key/SKILL.md)
- opencv: OpenCV computer vision library. Use for image processing. (file: r0/g1joshi/opencv/SKILL.md)
- opencv: OpenCV computer vision library. Use for image processing. (file: r0/g1joshi-opencv/SKILL.md)
- opencv: OpenCV computer vision library. Use for image processing. (file: r0/opencv/SKILL.md)
- opencv: OpenCV computer vision library. Use for image processing. (file: r0/opencv-g1joshi/SKILL.md)
- opencv: You are an expert in OpenCV (Open Source Computer Vision Library), the most popular library for real-time computer vision.  (file: r0/opencv-terminalskills/SKILL.md)
- pdf: Use when tasks involve reading, creating, or reviewing PDF files where rendering and layout matter; prefer visual checks (file: r0/pdf/SKILL.md)
- pdf:pdf: Read, create, inspect, render, and verify PDF files where visual layout matters. Use Poppler rendering plus Python tools (file: r7/pdf/26.715.12143/skills/pdf/SKILL.md)
- pptx-direct-edit: Directly edit PPTX slides without installing python-pptx. Use when the user asks to redesign tables, change colors, adjust (file: r1/pptx-direct-edit/SKILL.md)
- presentations:Presentations: Create or edit PowerPoint or Google Slides decks (file: r7/presentations/26.715.12143/skills/presentations/SKILL.md)
- ps: Use when the user says "ps", "图片", or asks to optimize, redesign, modularize, layer, split, or export Photoshop/PSD/image- (file: r0/ps/SKILL.md)
- qdn-project-vision: QDN / 全电脑模型工业视觉项目上下文。涉及四点 LED 光斑、solvePnP 位姿、K/D 内参、 T_shell_cam 标定、QT+OpenCV C++ 工控端视觉时必读。与通用 opencv skill 配合使用。 (file: r0/qdn-project-vision/SKILL.md)
- qt-upper-computer-ui-layout: Use when working on the user's Qt/C++ industrial upper-computer UI, especially qt-upper-computer-ui, P02/P07/P08/P10 pag (file: r0/qt-upper-computer-ui-layout/SKILL.md)
- radare2: Radare2 reverse-engineering workflow for binary analysis, disassembly, function discovery, strings, cross references, bi (file: r0/radare2/SKILL.md)
- reverse_engineer: Systematic binary reverse engineering toolkit. Extract source code, understand functions, document protocols, compare vers (file: r0/reverse_engineer/SKILL.md)
- safe-collab-workflow: Safe collaborative code-edit workflow for shared workspaces. Use when Codex needs to edit, repair, or verify code/files sa (file: r0/safe-collab-workflow/SKILL.md)
- safe-edit-gbk: 安全修改GBK编码的C文件（自动备份+Python修改+验证） (file: r0/safe-edit-gbk/SKILL.md)
- safe-restore: 安全的文件恢复流程（必须用户确认） (file: r0/safe-restore/SKILL.md)
- security-best-practices: Perform language and framework specific security best-practice reviews and suggest improvements. Trigger only when the u (file: r0/security-best-practices/SKILL.md)
- security-threat-model: Repository-grounded threat modeling that enumerates trust boundaries, assets, attacker capabilities, abuse paths, and mi (file: r0/security-threat-model/SKILL.md)
- senior-computer-vision: Computer vision engineering skill for object detection, image segmentation, and visual AI systems. Covers CNN and Vision T (file: r0/senior-computer-vision/SKILL.md)
- sites:sites-building: Use Sites to build websites, including landing pages, portfolios, dashboards, portals, trackers, hubs, and internal tool (file: r4/sites-building/SKILL.md)
- sites:sites-hosting: Host websites with Sites. Always use after `sites-building`, and use for website publishing, deployment, hosting managemen (file: r4/sites-hosting/SKILL.md)
- spreadsheets:Spreadsheets: Create, edit, analyze, and verify standalone spreadsheet files or Google Sheets-ready workbooks, including .xlsx, .xls, . (file: r8/spreadsheets/SKILL.md)
- spreadsheets:excel-live-control: Control an open or active Microsoft Excel workbook through the ChatGPT add-in or connected session. Use when the user tag (file: r8/excel-live-control/SKILL.md)
- survey-data-processor: Survey data processing for total-station, GNSS, leveling, point clouds, DTM/TIN generation, coordinate transformation, t (file: r0/survey-data-processor/SKILL.md)
- template-creator:template-creator: Create or update a reusable personal Codex artifact-template skill. Use when the user invokes $template-creator or asks  (file: r7/template-creator/26.715.12143/skills/template-creator/SKILL.md)
- total-station-calibration: Total station calibration and error-analysis workflow. Use for total-station instrument checks, collimation error, trunn (file: r0/total-station-calibration/SKILL.md)
- understand-anything:understand: Analyze a codebase to produce an interactive knowledge graph for understanding architecture, components, and relationships (file: C:/Users/t250c/.understand-anything/repo/understand-anything-plugin/skills/understand/SKILL.md)
- understand-anything:understand-chat: Use when you need to ask questions about a codebase or understand code using a knowledge graph (file: C:/Users/t250c/.understand-anything/repo/understand-anything-plugin/skills/understand-chat/SKILL.md)
- understand-anything:understand-dashboard: Launch the interactive web dashboard to visualize a codebase's knowledge graph (file: C:/Users/t250c/.understand-anything/repo/understand-anything-plugin/skills/understand-dashboard/SKILL.md)
- understand-anything:understand-diff: Use when you need to analyze git diffs or pull requests to understand what changed, affected components, and risks (file: C:/Users/t250c/.understand-anything/repo/understand-anything-plugin/skills/understand-diff/SKILL.md)
- understand-anything:understand-domain: Extract business domain knowledge from a codebase and generate an interactive domain flow graph. Works standalone (light (file: C:/Users/t250c/.understand-anything/repo/understand-anything-plugin/skills/understand-domain/SKILL.md)
- understand-anything:understand-explain: Use when you need a deep-dive explanation of a specific file, function, or module in the codebase (file: C:/Users/t250c/.understand-anything/repo/understand-anything-plugin/skills/understand-explain/SKILL.md)
- understand-anything:understand-knowledge: Analyze a Karpathy-pattern LLM wiki knowledge base and generate an interactive knowledge graph with entity extraction, imp (file: C:/Users/t250c/.understand-anything/repo/understand-anything-plugin/skills/understand-knowledge/SKILL.md)
- understand-anything:understand-onboard: Use when you need to generate an onboarding guide for new team members joining a project (file: C:/Users/t250c/.understand-anything/repo/understand-anything-plugin/skills/understand-onboard/SKILL.md)
- understand-first: 先理解任务再执行，打印理解内容等用户确认 (file: r0/understand-first/SKILL.md)
- univer-cli: Use when solving spreadsheet workbook problems with the `univer` or `unv` CLI as a terminal-native spreadsheet engine: Exc (file: r0/univer-cli/SKILL.md)
- verify-before-answer: 修改代码后立即验证，不敷衍用户 (file: r0/verify-before-answer/SKILL.md)
- viewtech-vt-parser: Inspect and carefully modify ViewTech/VGUS .vt HMI projects; decode page/address, signed or unsigned data type, integer an (file: r0/viewtech-vt-parser/SKILL.md)
- visual-plan: Turn ordinary text plans into rich interactive visual plans with diagrams, file maps, annotated code, open questions, an (file: r1/visual-plan/SKILL.md)
- visual-plan: Turn ordinary text plans into rich interactive visual plans with diagrams, file maps, annotated code, open questions, an (file: r0/visual-plan/SKILL.md)
- visualize:visualize: Create visualizations and interactive tools in conversation. Use when asked to show how something works, make simulators o (file: r3/visualize/1.0.12/skills/visualize/SKILL.md)
- work-continuity-sync: Triggered when the user says "上传" as a standalone command or asks for seamless work handoff across computers. Create and u (file: r0/work-continuity-sync/SKILL.md)
- workflow-memory-skillsmith: Mandatory workflow for turning repeated task lessons, tool discoveries, failure fixes, user-specific procedures, and pract (file: r0/workflow-memory-skillsmith/SKILL.md)
- xlsx: Use this skill any time a spreadsheet file is the primary input or output. This means any task where the user wants to: op (file: r0/xlsx/SKILL.md)
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
```

### user 2026-07-20T01:59:05.155Z

```text
<recommended_plugins>
Here is a list of plugins that are available but not installed.

- Atlassian Rovo (atlassian-rovo@openai-curated-remote)
- Box (box@openai-curated-remote)
- Figma (figma@openai-curated-remote)
- Gmail (gmail@openai-curated-remote)
- Google Calendar (google-calendar@openai-curated-remote)
- Google Drive (google-drive@openai-curated-remote)
- Notion (notion@openai-curated-remote)
- Outlook Calendar (outlook-calendar@openai-curated-remote)
- Outlook Email (outlook-email@openai-curated-remote)
- SharePoint (sharepoint@openai-curated-remote)
- Slack (slack@openai-curated-remote)
- Teams (teams@openai-curated-remote)
</recommended_plugins>
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
  <current_date>2026-07-20</current_date>
  <timezone>Asia/Shanghai</timezone>
  <filesystem><workspace_roots><root>E:\工作</root></workspace_roots><permission_profile type="managed"><file_system type="restricted"><entry access="read"><special>:root</special></entry><entry access="write"><path>E:\工作</path></entry><entry access="write"><special>:slash_tmp</special></entry><entry access="write"><special>:tmpdir</special></entry><entry access="write"><path>C:\Users\t250c\.codex\automations\automation</path></entry><entry access="read"><path>E:\工作\.git</path></entry><entry access="read"><path>E:\工作\.agents</path></entry><entry access="read"><path>E:\工作\.codex</path></entry></file_system></permission_profile></filesystem>
</environment_context>
```

### user 2026-07-20T01:59:05.215Z

```text
Automation: 每周工作状态更新
Automation ID: automation
Automation memory: $CODEX_HOME/automations/automation/memory.md
Last run: never

Review recent work activity in the workspace and draft a brief weekly status update. Summarize completed work, current in-progress items, blockers or risks, and suggested next steps. Keep it concise and ready to send or adapt.
```

### assistant 2026-07-20T01:59:12.659Z

```text
Reviewing recent workspace activity and the automation memory first, then I’ll pull the latest file and git signals to draft a concise weekly status update.
```

### assistant 2026-07-20T01:59:21.274Z

```text
The prior automation runs already show this workspace has been mostly dormant. I’m checking for any new file activity since the July 13 scan so the update reflects concrete dates rather than repeating stale status.
```

### assistant 2026-07-20T02:00:01.718Z

```text
I have enough evidence: nothing substantive changed after the July 13 scan, and the only newer timestamp is the `tmp\pdfs` directory on July 15 with no files under it. I’m recording this run in the automation memory, then I’ll return a send-ready weekly update.
```

### assistant 2026-07-20T02:00:33.960Z

```text
Weekly status update draft:

Completed work: No new completed work detected in [`E:\工作`](</E:/工作>) for the week ending July 20, 2026. The latest substantive artifacts in this workspace still date to May 25-27, 2026, including [`E:\工作\QtSmokeTest`](</E:/工作/QtSmokeTest>), [`E:\工作\QtOpenCVTemplate`](</E:/工作/QtOpenCVTemplate>), and [`E:\工作\全电脑CAN协议记录.html`](</E:/工作/全电脑CAN协议记录.html>).

In progress: No active implementation evidence was found in this workspace this week. A newer timestamp exists on `E:\工作\tmp\pdfs` from July 15, 2026, but there are no files under it, so it does not indicate substantive progress.

Blockers/risks: Status visibility is limited because [`E:\工作`](</E:/工作>) is not a usable git repository; `git status` fails, and the top-level `.git` appears to be placeholder-only. Repeated scans with no new artifacts suggest either this workspace is paused or current work has moved elsewhere.

Suggested next steps: Either repoint this automation to the active project path, or resume work here by generating a fresh build/run artifact so weekly reporting has concrete progress to capture.

I also appended this run summary to [`C:\Users\t250c\.codex\automations\automation\memory.md`](</C:/Users/t250c/.codex/automations/automation/memory.md>).

::inbox-item{title="Weekly workspace status drafted" summary="No new activity found; repoint or resume active work"}
```
