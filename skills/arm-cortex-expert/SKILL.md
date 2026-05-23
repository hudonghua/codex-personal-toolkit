---
name: arm-cortex-expert
description: ARM Cortex-M embedded development guidance for startup, vector table, NVIC, SysTick, low power, DMA, cache coherency, MPU, fault diagnosis, GPIO/peripheral registers, STM32/nRF/SAMD/RP2040-style MCUs, and real-time firmware debugging.
---

# ARM Cortex Expert

Use this skill for ARM Cortex-M MCU development, especially STM32, nRF, SAMD, LPC, GD32, CH32, RP2040, and similar embedded firmware. It complements `keil5-embedded-c` and `mcu-gpio-peripheral`.

## First Identify

- Exact MCU part number.
- Core: M0/M0+/M3/M4/M7/M23/M33/M55.
- Toolchain: Keil/MDK, IAR, GCC, vendor IDE.
- HAL/LL/bare-metal style.
- Clock tree and system clock.
- Linker/scatter file and flash/RAM layout.
- Interrupt source and expected timing.

## Core Concepts

- Vector table starts with initial stack pointer and reset handler.
- Cortex-M code uses Thumb instruction set.
- NVIC priority grouping matters; lower numeric priority means higher urgency.
- SysTick is often used for RTOS tick or millisecond timebase.
- Fault handlers are diagnostic gold: HardFault, MemManage, BusFault, UsageFault.
- On M7/M33-class parts, cache and MPU can affect DMA and peripheral buffers.

## GPIO / Peripheral Register Rules

- Enable peripheral clock before touching registers.
- Configure pin mode before enabling output drivers when possible.
- Check alternate function mapping table, not just pin name.
- Confirm voltage domain and tolerance.
- Configure pullup/pulldown based on external circuit.
- For open-drain outputs, ensure a pullup exists.
- For interrupts, clear pending flags in the correct peripheral and NVIC path.

## Interrupts

- Keep ISRs short.
- Avoid blocking calls, dynamic allocation, and long printf in ISRs.
- Use volatile flags or queues to hand work to the main loop/task.
- Confirm interrupt flag clear sequence from reference manual.
- Avoid priority inversion between DMA, timers, UART, and RTOS syscall priorities.

## DMA And Cache

For DMA buffers:

- Align buffers according to cache line requirements.
- Clean data cache before DMA transmit.
- Invalidate data cache after DMA receive.
- Put DMA buffers in non-cacheable RAM when the MCU supports it.
- Do not assume `volatile` fixes cache coherency.

## HardFault Debug Checklist

- Capture stacked registers: R0-R3, R12, LR, PC, xPSR.
- Decode CFSR, HFSR, BFAR, MMFAR when present.
- Check stack overflow.
- Check invalid function pointer or corrupted vector table.
- Check unaligned access, divide-by-zero, bad peripheral address, or executing data.
- Check interrupt priority misuse with RTOS APIs.

## Low Power

- Confirm wake sources.
- Disable unused peripheral clocks.
- Configure GPIO states to avoid leakage.
- Account for debug probe keeping the chip awake.
- Measure current on real hardware; datasheet numbers assume ideal board design.

## Response Style

When helping with Cortex-M firmware:

- Cite the exact MCU assumption.
- Separate register-level facts from HAL convenience calls.
- Ask for schematic/pinout/reference-manual info when pin behavior matters.
- For bugs, trace from clock enable to GPIO/peripheral config to ISR/DMA/data consumer.
