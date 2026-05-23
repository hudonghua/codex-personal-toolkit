---
name: radare2
description: Radare2 reverse-engineering workflow for binary analysis, disassembly, function discovery, strings, cross references, binary diffing, and safe patch planning. Use when analyzing or patching executables, firmware blobs, ELF/PE/Mach-O files, or raw binaries with r2/rabin2/rahash2/radiff2/rasm2.
---

# Radare2 Reverse Engineering

Use this skill when the user wants to analyze a binary with radare2 or needs a systematic CLI workflow for strings, functions, disassembly, cross references, binary diffing, checksums, or patch planning.

This local skill is based on the publicly indexed `dnakov/radare2` skill summary and expanded for Codex use.

## Safety First

- Work on a copy, never the only original binary.
- Record file hash before and after any analysis or patch.
- Prefer read-only analysis first. Use write mode only after making a backup.
- Do not execute unknown firmware or binaries on the host system.
- For patching, document address, original bytes, new bytes, reason, and verification.

## Quick Start

```bash
r2 -A binary        # open with auto-analysis
r2 binary          # open without heavy analysis
r2 -w binary       # write mode for patching, only on a backup
r2 -d binary       # debug mode for native executable debugging
```

Useful companion tools:

```bash
rabin2 -I binary       # file info
rabin2 -z binary       # strings
rabin2 -S binary       # sections
rabin2 -s binary       # symbols
rahash2 binary         # hashes
radiff2 old.bin new.bin
rasm2 -a arm -b 16 'nop'
```

## Initial Triage

```bash
file binary
rahash2 -a md5,sha1,sha256 binary
rabin2 -I binary
rabin2 -S binary
rabin2 -z binary
```

For raw MCU firmware, `rabin2 -I` may not know the architecture. Ask for:

- MCU family.
- Flash base address.
- Vector table layout.
- Endianness.
- Thumb/ARM/8051/AVR/STM8 mode.

## Essential r2 Commands

Inside `r2`:

```text
aaa          analyze all
afl          list functions
s addr       seek to address
s main       seek to main
pdf          print disassembly of function
pdc          print decompiler-like output if available
izz          list strings
axt addr     find cross references to address
axf addr     find references from address
VV           visual graph mode
q            quit
```

Search:

```text
/ string
/x 12 34 56 78
/R pop
```

Patch planning:

```text
px 32 @ addr          print 32 bytes at address
wa nop @ addr         write assembled instruction
wx 00 00 @ addr       write raw bytes
wc                   list write cache
wcj                  write cache as JSON
wcc                  commit write cache
```

Use write-cache review before committing patches.

## Function Analysis Workflow

1. Identify architecture and load address.
2. Run analysis.
3. List functions.
4. Anchor on strings or constants.
5. Use cross references to find code that uses those strings/constants.
6. Read disassembly and note branch conditions.
7. Document evidence by address.
8. If patching, first prove the target condition and exact byte range.

## Binary Diff Workflow

```bash
radiff2 old.bin new.bin
radiff2 -x old.bin new.bin
radiff2 -s old.elf new.elf
```

For firmware:

- Compare raw bytes first.
- If files contain version metadata, ignore expected version string areas only after identifying them.
- Watch for checksum/CRC changes near file end or headers.
- If many bytes shifted, check whether a section moved or compression/encryption changed.

## Raw Firmware Notes

For raw `.bin`, addresses in the file are offsets, not necessarily runtime addresses.

Keep a mapping:

```text
runtime_address = file_offset + flash_base
file_offset = runtime_address - flash_base
```

Common flash bases:

- ARM Cortex-M internal flash often starts at `0x08000000`, but verify per MCU.
- Vector table at offset `0x00000000` often contains initial stack pointer and reset handler.

## Output Format

When reporting:

- File name and hashes.
- File type and guessed architecture.
- Key strings/symbols/sections.
- Address map used.
- Functions or offsets inspected.
- Evidence for each conclusion.
- Patch table if modified:

```text
offset/runtime address | old bytes | new bytes | purpose | verification
```

## Boundaries

Use this skill for legitimate analysis, repair, compatibility, migration, and user-owned firmware. Do not assist with malware deployment, credential theft, license bypass, or unauthorized access.
