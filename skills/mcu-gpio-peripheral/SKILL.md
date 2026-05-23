---
name: mcu-gpio-peripheral
description: MCU GPIO and peripheral bring-up workflow for embedded C/Keil/MDK projects. Covers input/output, pullup/pulldown, push-pull/open-drain, alternate functions, EXTI interrupts, debounce, ADC/PWM/UART/I2C/SPI pin conflicts, schematics, register/HAL checks, and Chinese embedded-code conventions.
---

# MCU GPIO Peripheral

Use this skill for chip GPIO and peripheral development: GPIO input/output, 上拉/下拉, 推挽/开漏, 复用功能, 外部中断, 消抖, ADC, PWM, UART, I2C, SPI, CAN, RS485, timer capture/compare, and board-level pin conflict checks.

Pair this with:

- `keil5-embedded-c` for Keil/MDK code editing and GBK/Chinese comments.
- `arm-cortex-expert` for Cortex-M core/NVIC/DMA/cache/fault issues.
- `esp32` for ESP32-specific GPIO traps.

## First Questions

Before changing GPIO/peripheral code, identify:

- MCU model and package.
- Pin name and physical pin number.
- Schematic connection.
- External circuit: pullup, pulldown, resistor, transistor, relay, optocoupler, sensor, bus, LED, button.
- Signal active level: active-high or active-low.
- Voltage level and tolerance.
- Desired mode: input, output, analog, alternate function, interrupt.
- Existing code path and execution period.

## GPIO Mode Checklist

For each pin, confirm:

- Clock enabled for GPIO port.
- Mode: input, output, analog, alternate function.
- Output type: push-pull or open-drain.
- Pull: none, pullup, pulldown.
- Speed/drive strength.
- Initial output level before enabling external load when needed.
- Alternate function number.
- Lock or remap settings if the MCU has them.

## Push-Pull vs Open-Drain

Use push-pull when the MCU actively drives both high and low and no bus sharing is required.

Use open-drain when:

- I2C bus.
- Wired-OR / shared interrupt line.
- Level shifting with pullup.
- External device expects open collector/drain.

Open-drain requires a pullup. Internal pullup may be too weak for fast edges or bus use.

## Input Pulls

- Floating input causes random readings.
- Use internal pullup/pulldown only when current and noise requirements allow.
- Buttons often need debounce and active-level confirmation.
- Long wires need stronger biasing, filtering, shielding, or optocoupling.

## Interrupt / EXTI

For GPIO interrupt:

1. Configure pin input mode and pull.
2. Configure edge: rising, falling, or both.
3. Clear pending flag before enabling.
4. Enable EXTI/peripheral interrupt.
5. Enable NVIC line with correct priority.
6. In ISR, confirm source flag, clear flag, then set a short software flag.
7. Do debounce outside ISR when possible.

Never put long delays or heavy logic inside GPIO ISR.

## Debounce

Button debounce patterns:

- Time filter in periodic task.
- State machine with stable-count threshold.
- Interrupt only wakes the system, then main loop confirms stable state.

Avoid:

- Blocking delay in ISR.
- Treating one edge as final when mechanical contacts bounce.

## ADC Pins

- Configure analog mode.
- Disable digital input buffer if MCU recommends it.
- Confirm channel mapping.
- Check source impedance and sampling time.
- Add RC filtering when needed.
- Calibrate ADC if required.
- Convert raw counts with reference voltage and divider ratio.

## PWM Pins

- Confirm timer channel and alternate function mapping.
- Configure timer clock, prescaler, period, duty.
- Set output polarity.
- Enable preload if updating duty during runtime.
- Start timer output after GPIO AF is configured.
- For motor/driver control, set safe default level before enabling output.

## UART / RS485

- Confirm TX/RX not swapped.
- Confirm alternate function/remap.
- Confirm baud, parity, stop bits.
- For RS485, control DE/RE timing.
- Add timeout and frame validation.

## I2C

- Open-drain with pullups.
- Check bus voltage.
- Confirm address 7-bit vs 8-bit notation.
- Scan bus only as a diagnostic, not as final proof.
- Handle NACK, arbitration loss, stuck bus, and clock stretching if applicable.

## SPI

- Confirm CPOL/CPHA.
- Confirm bit order.
- Confirm CS polarity and timing.
- Keep CS as normal GPIO unless hardware NSS behavior is desired.
- Watch shared bus conflicts and max clock.

## Pin Conflict Workflow

When a pin does not work:

1. Check schematic and package pinout.
2. Check if the pin is used by boot, debug, oscillator, flash, USB, or another peripheral.
3. Check clock enable.
4. Check mode and alternate function.
5. Check external pull/drive/load.
6. Check active level.
7. Check code writes/reads the correct port and pin.
8. Check ISR or periodic task overwrites the pin.
9. Measure with multimeter or oscilloscope if possible.

## Chinese Embedded C Style

- Preserve Chinese comments and file encoding.
- Keep variable naming consistent with project style.
- Prefer simple state flags, counters, and clear branches in old MDK C projects.
- Avoid unnecessary structs, dynamic allocation, or modern C features unless the project already uses them.

## Output Format

For GPIO/peripheral answers, include:

- Pin table: port/pin, mode, pull, output type, AF/channel, active level, external circuit.
- Initialization order.
- Runtime path: where value is read/written, ISR or polling period.
- Risks: boot pin, debug pin, analog-only, input-only, shared peripheral.
- Verification: expected voltage/waveform/log/register state.
