# Keil5 Reference

Use this reference as a quick lookup for common Keil uVision5 project artifacts and investigation targets.

## NXP LPC1765 and LPC17xx Clues

When the target is NXP LPC1765 or another LPC17xx device, search for:

- `LPC1765`, `LPC176x`, `LPC17xx`
- `startup_LPC17xx.s`
- `system_LPC17xx.c`
- `LPC17xx.h`
- `SystemInit`
- `SystemCoreClockUpdate`
- `PLL0`, `CCLKCFG`, `PCLKSEL0`, `PCLKSEL1`, `PCONP`

These projects commonly rely on CMSIS startup plus NXP device headers rather than STM32-style HAL layout.

## Common File Types

- `.uvprojx`: main project definition, typically contains targets, groups, source membership, include paths, macros, output settings, and device selection
- `.uvoptx`: user options such as debug session state; useful for context, but usually not authoritative for build logic
- `.uvguix.<user>`: per-user IDE layout and UI state
- `.uvmpw`: multi-project workspace file
- `.sct`: Arm Compiler scatter-loading file for memory layout
- `startup_*.s`: startup assembly, vector table, reset handler
- `system_*.c`: system clock and low-level init, often includes `SystemInit` and `SystemCoreClockUpdate`
- `RTE/`: Run-Time Environment generated content and pack-managed configuration
- `LPC17xx.h`: NXP LPC17xx CMSIS device header
- `startup_LPC17xx.s`: common LPC17xx startup assembly name in Keil projects
- `system_LPC17xx.c`: common LPC17xx system clock/init file

## High-Value XML Tags in `.uvprojx`

Search these first when tracing configuration:

- `<TargetName>`
- `<Device>`
- `<Vendor>`
- `<Cpu>`
- `<OutputName>`
- `<OutputDirectory>`
- `<ListingPath>`
- `<ScatterFile>`
- `<IncludePath>`
- `<Define>`
- `<FilePath>`
- `<FileName>`

## Typical Embedded Layers

Many Keil projects are organized into some combination of:

- app or application logic
- BSP board support
- CMSIS core and device headers
- HAL or LL drivers
- middleware such as USB, filesystem, network, or RTOS
- startup and linker configuration

Map ownership before editing so vendor or generated code is not modified unnecessarily.

For LPC1765 projects, common ownership boundaries are:

- CMSIS startup and system files from NXP or Keil packs
- peripheral driver files for UART, GPIO, TIMER, ADC, PWM, I2C, SPI, CAN, Ethernet, or USB
- board-specific glue for clocks, LEDs, keys, relays, or external buses
- app logic layered above register or driver wrappers

## Common Failure Classes

- target include path missing a folder that exists on disk
- source file exists but is not included in the active target
- symbol name mismatch between startup file and C handler implementation
- scatter file or memory region mismatch after code size changes
- device pack version mismatch versus checked-in source
- duplicate driver or middleware source pulled into multiple groups
- preprocessor macro mismatch between debug and release targets
- wrong clock assumptions causing baud rate, timer, or delay misbehavior on LPC1765
- ISR implementation name not matching the LPC17xx startup vector symbol
- peripheral setup split across `SystemInit`, board init, and app init in inconsistent ways

## Useful Investigation Questions

- Which `.uvprojx` is the active product project?
- Which target is intended for the requested behavior?
- Which file owns reset, clock, interrupt, or memory behavior?
- Is the failing file part of the active target?
- Is the fix app-owned, generated, or vendor-owned?
- Can the problem be proven from project config without hardware access?
