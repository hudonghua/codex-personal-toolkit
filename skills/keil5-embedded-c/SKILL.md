---
name: keil5-embedded-c
description: "Keil5 and embedded C firmware workflow for hardware-related work. Use when Codex works on Keil/uVision projects, C51/ARM/MCU/单片机 code, GBK C files, timers/定时器, interrupts/中断, IO/ADC/PWM/CAN/UART/RS485 protocols/硬件协议, screen/屏端 integration, bin/hex verification, protocol documents/协议文档, or embedded C code changes where correctness depends on real code paths and device behavior."
---

# Keil5 Embedded C

Use this skill for Keil5/uVision firmware, embedded C, MCU-side logic, hardware protocols, timing, interrupts, IO, ADC, PWM, CAN, UART, RS485, screen/MCU integration, and protocol documents.

## Operating Rules

- Treat hardware work as exact: protocol, timing, polarity, register, and frame mistakes are functional failures.
- Prefer evidence from actual code, project files, binaries, logs, captures, and call paths over names or guesses.
- State uncertainty plainly. Do not claim a fix is complete until it has been verified.
- Keep changes small, direct, and consistent with the existing project style.
- Preserve the user's naming style and simple-variable embedded C style. Avoid unnecessary pointers, arrays, structs, enums, framework abstractions, and clever generalization.

## Before Editing

1. Identify the target project and relevant Keil files: `.uvproj`, `.uvprojx`, `.uvopt`, `.uvoptx`, startup files, scatter/linker files, include paths, defines, and the actual `.c`/`.h` compile units.
2. Explain the intended work before changing hardware, protocol, timing, or embedded control logic:
   - task goal
   - files likely to change
   - data flow and call path
   - execution period or interrupt source
   - verification method
3. Back up every existing file before modifying it. Do not edit if the backup fails.
4. Detect and preserve file encoding. For GBK C files, do not rewrite as UTF-8. Use an encoding-preserving method and re-check Chinese comments after editing.
5. Use `rg`/`rg --files` first for search. Search by symbol, call site, flag assignment, frame ID, register name, page ID, and protocol byte patterns.

## Editing C Safely

- Lock the complete function boundary before editing a function:
  1. find the exact function header
  2. match the corresponding closing brace
  3. edit only inside that range
  4. inspect the next function after editing to confirm it was not swallowed or damaged
- For interrupt handlers, scheduler functions, polling loops, and protocol parsers, inspect upstream and downstream callers before changing behavior.
- Do not infer execution period from a function name such as `app10ms()`. Trace the real trigger: timer configuration, ISR, flag set location, scheduler condition, and main-loop consumption.
- When adding counters or delays, base them on the confirmed real period, not a guessed function name.
- Keep MCU/screen responsibilities separate in screen projects. Modify screen pages through the established page path and MCU logic through MCU-side files.

## Hardware And Protocol Work

- For CAN/UART/RS485/SPI/I2C/proprietary frames, verify:
  - byte order
  - scale factors
  - signed/unsigned interpretation
  - checksum/CRC
  - frame length
  - ID/address
  - timeout and retry behavior
  - receive and transmit call paths
- For IO/ADC/PWM/control logic, verify:
  - active high/low polarity
  - filtering/debounce
  - sampling period
  - unit conversion
  - limit and fault behavior
  - hardware pin or channel mapping
- For generated protocol documents, read the real code first and cite source file locations where practical. Do not invent fields from expected behavior.

## Validation

- After edits, verify with the strongest available method:
  - Keil build output if build tools are available
  - static inspection of touched function boundaries
  - symbol/call-path search
  - protocol frame comparison against actual code or binary data
  - targeted reasoning from timer/ISR to consuming logic
- If Keil cannot be run locally, say so and perform file-level validation instead.
- When the user asks "are you sure?", re-check the evidence rather than repeating the earlier answer.

## Response Style

- For embedded/hardware changes, report what was verified and what could not be verified.
- Include exact file paths and line references when explaining code.
- Say "not verified" for anything not directly checked.
- Avoid broad rewrites and unrelated cleanup unless the user explicitly asks.
