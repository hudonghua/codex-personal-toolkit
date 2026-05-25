---
name: keil5
description: Work with Keil5, MDK-ARM, and uVision embedded projects, including NXP LPC1765 and other LPC17xx targets. Use when Codex needs to inspect `.uvprojx` or `.uvoptx` projects, trace target and device settings, understand startup or scatter/linker configuration, edit embedded C/H source used by Keil builds, diagnose build failures, or explain how a Keil5 project is structured and built.
---

# Keil5

Use this skill for Keil uVision5 and MDK-ARM workspaces where project structure, target configuration, startup files, and toolchain outputs matter as much as the source edits. This includes NXP LPC1765 and nearby LPC17xx projects that use CMSIS startup code, vendor peripheral drivers, and Keil scatter-loading.

Load [references/keil5-reference.md](./references/keil5-reference.md) when you need quick reminders about common Keil file types, build artifacts, or search targets.

## Workflow

### 1. Establish the project entry point

Start by locating the project files before making assumptions about the build:

- search for `*.uvprojx`, `*.uvoptx`, `*.uvguix.*`, `*.uvmpw`
- search for linker and memory layout files such as `*.sct`, `*.scf`, `*.ld`
- search for startup and system files such as `startup_*.s`, `system_*.c`, `stm32*.s`, `system_stm32*.c`, `startup_LPC17xx.s`, `system_LPC17xx.c`
- identify whether the repo is single-target or multi-target

If multiple `.uvprojx` files exist, map which one is the real product build and which are demos, test targets, or archived variants.

### 2. Read configuration before editing code

Treat Keil projects as configuration-heavy:

- inspect the `.uvprojx` to identify target names, device family, output path, include paths, preprocessor macros, and source group membership
- inspect scatter or linker config before changing memory-sensitive code
- inspect startup files before changing interrupt names, vector table content, reset flow, or low-level init
- inspect `RTE/`, CMSIS, HAL, BSP, and middleware folders before assuming ownership of a file

For LPC1765 and LPC17xx specifically:

- check whether the project uses CMSIS device headers such as `LPC17xx.h`
- check whether clock setup flows through `SystemInit`, PLL setup, and `SystemCoreClockUpdate`
- check whether peripheral code comes from NXP legacy driver libraries, CMSIS examples, or app-local wrappers

Do not assume a source file participates in the active build just because it exists in the repo. Confirm it is included by the target or pulled through the build system.

### 3. Match the edit method to embedded constraints

For code edits:

- restate the requested behavior and the likely files to change
- trace the call path, ISR path, scheduler path, or state machine path before modifying timing-sensitive logic
- preserve file encoding and existing line endings when possible
- keep diffs local and avoid broad refactors unless the user asked for them
- respect existing register naming, macro style, and section placement

Treat brace safety as a primary failure mode in these projects:

- do not insert statements into a function until the surrounding block structure is re-read from the function signature down to its closing brace
- do not assume a nearby `}` belongs to the current `if`, `while`, or function; trace the full nesting first
- when changing a branch body, prefer editing the smallest complete block instead of splicing lines into the middle of partially understood code
- after every edit, reopen the whole affected function and verify that each opening brace still matches the intended scope
- if the file already has suspicious indentation or mismatched local structure, stop and repair the structure before adding new logic

For legacy or risky C/H edits, combine this skill with `$safe-collab-workflow`, `$backup-and-edit`, or GBK-safe skills when the source tree warrants it.

### 4. Verify against Keil realities

After making changes, verify in the most direct way available:

- reopen the changed code and confirm the final text is present
- search for duplicate definitions, conflicting macros, and ISR name mismatches
- if a local build command is available, run it
- if a build is not available, validate by tracing target config, include paths, and referenced symbols
- report any uncertainty around missing toolchain, pack versions, or device-specific headers

Do not claim a build fix is complete if you only changed source text without checking whether the target actually compiles under the active Keil configuration.

## Task Patterns

### Inspect a Keil project

When asked to explain a project:

- identify the main `.uvprojx`
- list targets and infer debug or release variants
- identify MCU or device family
- identify startup file, system init file, and linker/scatter file
- identify major middleware layers such as CMSIS, HAL, RTOS, BSP, drivers, app
- summarize the build flow from reset to `main`

For LPC1765, also call out:

- the exact device string from `.uvprojx`
- whether the project uses `startup_LPC17xx.s` and `system_LPC17xx.c`
- whether clocking depends on external crystal assumptions or PLL constants
- whether drivers touch common LPC17xx blocks such as GPIO, UART, TIMER, ADC, CAN, or Ethernet

### Diagnose build failures

When asked to fix a build:

- capture the exact error text first
- locate the failing file, symbol, or include
- verify whether the file is part of the active target
- inspect target macros and include paths in `.uvprojx`
- inspect whether the error is caused by pack mismatch, missing generated code, wrong target selection, or duplicate source inclusion

Prefer root-cause fixes over patching around missing symbols with ad hoc declarations.

### Modify startup, memory, or interrupt logic

Use extra caution when the request touches:

- vector table entries
- reset handler flow
- `SystemInit`
- scatter-loading or RAM/ROM layout
- bootloader handoff
- section attributes, `__attribute__`, `__irq`, `__weak`, or compiler-specific qualifiers
- LPC17xx clock tree, PLL, peripheral clock dividers, or VPB/peripheral enable logic

Trace the real ownership and startup sequence before changing these areas.

### Insert logic into existing functions

This is the highest-risk edit pattern for brace mistakes. When adding code inside an existing function:

- identify the exact function start and function end first
- map the local control blocks in order: `if`, `else`, `switch`, `for`, `while`, and callback or macro-wrapped blocks
- prefer replacing a small complete block with an updated complete block instead of inserting isolated lines near braces
- verify the final function by reading it top to bottom immediately after editing
- if practical, run a targeted search or build check before considering the change done

### Work with generated or vendor code

Keil projects often mix handwritten code with generated or vendor-managed files. Before editing:

- decide whether the file is generated, vendor-supplied, or app-owned
- avoid edits that are likely to be overwritten by code generators unless the user asked for that tradeoff
- prefer wrapper hooks, user sections, or app-layer fixes when available

## Search Hints

Use fast repo searches to build context:

```powershell
rg --files -g "*.uvprojx" -g "*.uvoptx" -g "*.sct" -g "startup_*" -g "system_*.c"
rg -n "<TargetName>|<Device>|<ScatterFile>|<IncludePath>|<Define>" project.uvprojx
rg -n "Reset_Handler|SystemInit|SysTick_Handler|HardFault_Handler" .
rg -n "__irq|__weak|NVIC|SCB->VTOR|SystemCoreClock" .
rg -n "LPC1765|LPC17xx|SystemCoreClockUpdate|PLL0|PCLKSEL|PCONP|startup_LPC17xx" .
```

If `rg` is unavailable, fall back to PowerShell file search and `Select-String`.

## Guardrails

Do not:

- assume GCC, CMake, or Makefile behavior applies to a Keil-only project
- rename interrupts or startup symbols without checking vector table linkage
- change memory layout blindly without reading the scatter file
- assume every file in the tree belongs to the active target
- "fix" missing includes without checking target include paths and preprocessor macros
- overwrite generated or vendor code casually when there is an app-layer alternative
- insert code next to function or branch braces without re-reading the full enclosing function
- trust indentation alone when deciding where a closing brace belongs

## Output Style

During work, keep updates concrete:

- what project file was identified
- what target or device was found
- what files are likely affected
- what was verified versus inferred

After completion, summarize the changed behavior, project-specific evidence, and any remaining build or hardware-side uncertainty.
