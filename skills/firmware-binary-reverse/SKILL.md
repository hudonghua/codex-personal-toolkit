---
name: firmware-binary-reverse
description: MCU and embedded firmware binary workflow for .bin/.hex/.elf/.axf analysis, byte patching, version comparison, checksum/CRC repair, Keil/MDK firmware verification, fixed-address parameter edits, and safe documentation. Use for user-owned firmware images and pair with keil5-embedded-c for source-level work.
---

# Firmware Binary Reverse

Use this skill for embedded firmware binary work: `.bin`, `.hex`, `.elf`, `.axf`, `.s19`, `.mot`, `.map`, `.lst`, and generated Keil/MDK artifacts. It is focused on practical MCU firmware analysis and careful binary modification, not broad malware analysis.

Use with `keil5-embedded-c` when source code or Keil projects are available. Use with `reverse_engineer` or `radare2` when deeper disassembly is needed.

## Operating Rules

- Preserve the original file exactly.
- Create a timestamped backup before any modification.
- Calculate hashes before and after edits.
- Never edit a binary without recording offset, original bytes, new bytes, and reason.
- Distinguish file offset from runtime address.
- Verify checksum/CRC rules before changing bytes in images that contain integrity checks.
- Treat all architecture, flash base, endian, and vector table assumptions as unverified until proven.
- If the task involves a third-party device, confirm the user owns it or is authorized to analyze it.

## First Questions

Ask or infer:

- File type: raw BIN, Intel HEX, Motorola S-record, ELF, AXF, update package, or bootloader image.
- Target MCU/CPU: ARM Cortex-M, 8051/C51, STM8, AVR, MIPS, RISC-V, etc.
- Toolchain: Keil/MDK, IAR, GCC, vendor IDE, or unknown.
- Flash base address and image load address.
- Whether a `.map`, `.lst`, `.hex`, `.elf`, `.axf`, source tree, or protocol document exists.
- What needs to change: parameter, string, version, threshold, table, jump, checksum, or protocol field.
- Whether device has bootloader validation, signature, CRC, or encrypted/compressed package.

## File Handling

Recommended backup naming:

```text
firmware.bin
firmware.bin.orig
firmware.bin.before_patch_YYYYMMDD_HHMMSS
```

Recommended record:

```text
file:
sha256_before:
sha256_after:
offset:
runtime_address:
old_bytes:
new_bytes:
reason:
verification:
```

## Triage Workflow

1. Identify file type.
2. Compute hashes and file size.
3. Look for text strings and version strings.
4. If HEX/S-record, convert to binary only after preserving the original.
5. If ELF/AXF, inspect sections, symbols, and load addresses.
6. If raw BIN, determine flash base and architecture.
7. Check for obvious headers, magic values, compression, encryption, or CRC footer.
8. Search for the value/string/table to modify.
9. Validate all candidate locations before patching.

## Common Tools

Use available tools when installed:

```text
certutil -hashfile file.bin SHA256       Windows hash
Format-Hex file.bin                      quick hex view in PowerShell
fc /b old.bin new.bin                    byte comparison on Windows
srec_cat                                 hex/bin/srec conversion
fromelf                                  Keil/ARM artifact extraction when available
objdump / readelf                        ELF inspection
rabin2 / r2 / radiff2                    radare2 analysis
Ghidra                                   deeper reverse engineering
binwalk                                  packed Linux/IoT firmware extraction
```

If a tool is missing, do not invent results. Use the next available method and state the limitation.

## Address Mapping

For raw firmware:

```text
runtime_address = flash_base + file_offset
file_offset = runtime_address - flash_base
```

Common example:

```text
flash_base = 0x08000000
file_offset = 0x1234
runtime_address = 0x08001234
```

Do not assume `0x08000000` for every MCU. Confirm from linker script, map file, vector table, datasheet, or project settings.

## Intel HEX / S-Record

For Intel HEX:

- Preserve line checksums.
- Do not hand-edit lines unless checksum is recalculated.
- Convert to BIN for byte search when useful, then regenerate HEX using a structured tool.
- Track extended linear address records.

For S-record:

- Preserve record type and address width.
- Recalculate record checksums.

## Checksum And CRC

Before patching, look for:

- Simple additive checksum.
- CRC16, CRC32, or vendor-specific CRC.
- Header checksum.
- Footer checksum.
- Bootloader metadata block.
- Cryptographic signature.

If cryptographic signing is present, ordinary patching may make the image unbootable.

CRC workflow:

1. Identify covered byte range.
2. Identify polynomial/init/xor/reflection if possible.
3. Confirm by reproducing existing CRC.
4. Patch payload.
5. Recompute CRC.
6. Verify image with the same calculation.

## Fixed-Address Parameter Edits

Use only when the address is confirmed by one of:

- Map file symbol.
- Source code constant.
- Repeated version comparison.
- Known table structure.
- Disassembly reference.
- Prior verified record from the user.

Patch record must include:

```text
offset
old value
new value
data type
endianness
scale factor
source of confidence
```

## Version Comparison

When comparing two firmware versions:

1. Hash both files.
2. Compare file size.
3. Generate byte diff.
4. Identify changed regions.
5. Classify changes:
   - version string
   - date/build metadata
   - parameter table
   - code region
   - checksum/footer
   - shifted/compressed/encrypted region
6. Avoid patching by copying large blocks unless the region meaning is known.

## Keil / MDK Artifacts

Useful files:

- `.uvprojx` / `.uvoptx`: project settings.
- `.map`: symbol addresses and memory layout.
- `.lst`: assembly listing.
- `.hex`: Intel HEX output.
- `.axf` / `.elf`: symbols and sections.
- `.bin`: raw image.

Prefer symbol/map-based edits over blind byte search when these files exist.

## 8051 / C51 Notes

- Code memory and data memory are separate address spaces.
- Do not assume linear ARM-style memory.
- Pay attention to bit-addressable memory, sfr names, and code banking.
- Function pointers and jump tables may use compiler-specific layouts.

## ARM Cortex-M Notes

- Vector table starts with initial MSP and reset handler.
- Thumb addresses often have bit 0 set in vector entries.
- Branch patching must respect Thumb instruction width, alignment, and range.
- Replacing instructions with NOP requires correct architecture mode.

## Personal Records

When the user provides prior notes, convert them into a `Records` section or a separate reference file under this skill. Keep each record structured:

```text
Device / project:
MCU:
File type:
Known flash base:
Patch purpose:
Offset / address:
Original bytes:
New bytes:
Checksum method:
Verification result:
Date/source:
```

Do not merge personal records into generic rules unless repeated evidence supports them.

## Report Style

For every binary task, report:

- What was inspected.
- What was modified, if anything.
- Exact offset/address and bytes.
- Backup file name.
- Hashes before/after.
- Verification performed.
- What remains unverified.

## Boundaries

Allowed:

- User-owned firmware repair, migration, compatibility checks, parameter edits, regression comparison, checksum repair, and protocol understanding.

Not allowed:

- Credential theft, unauthorized access, malware modification, license bypass, DRM bypass, or instructions to compromise third-party systems.
