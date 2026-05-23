---
name: esp32
description: ESP32 embedded development guidance focused on GPIO conflicts, strapping pins, ADC2 with WiFi, deep sleep wake pins, FreeRTOS pitfalls, memory, peripherals, OTA, and power stability.
---

# ESP32

Use this skill when working on ESP32, ESP32-S2, ESP32-S3, ESP32-C3, ESP32-C6, ESP32-H2, ESP-IDF, Arduino-ESP32, FreeRTOS, GPIO selection, WiFi, deep sleep, ADC, PWM/LEDC, UART, SPI, I2C, OTA, and power issues.

This local skill is based on the publicly indexed `openclaw/esp32` skill and expanded for Codex use.

## Board First

Always identify the exact chip and board before assigning pins. ESP32 variants have different GPIO maps and boot strapping behavior.

Ask for:

- Chip variant: ESP32, S2, S3, C3, C6, H2, P4, etc.
- Board name and schematic if available.
- Framework: ESP-IDF, Arduino, PlatformIO, ESPHome, vendor SDK.
- Required peripherals: WiFi, BLE, ADC, PWM, I2C, SPI, UART, camera, SD card, PSRAM.

## GPIO Restrictions

- GPIO6-GPIO11 are usually connected to SPI flash on classic ESP32. Do not use them.
- GPIO34-GPIO39 are input-only on classic ESP32. They cannot output and usually have no internal pullup/pulldown.
- Strapping pins affect boot mode. Be careful with GPIO0, GPIO2, GPIO4, GPIO5, GPIO12, GPIO15 depending on variant and board.
- ADC2 conflicts with WiFi on classic ESP32. Use ADC1 pins when WiFi is active.
- Some pins may already be used by USB, PSRAM, flash, camera, SD card, onboard LEDs, buttons, or boot circuitry.

## ADC

- Prefer ADC1 when WiFi is used.
- Treat ADC readings as approximate unless calibrated.
- Check attenuation and full-scale range.
- Use filtering or averaging for noisy analog inputs.
- Watch source impedance; ADC sampling can be unstable with high-impedance sources.

## PWM / LEDC

- Classic Arduino `analogWrite()` may not map to hardware PWM as expected.
- Use LEDC APIs in ESP-IDF or Arduino-ESP32:

```c
ledcSetup(channel, frequency, resolution_bits);
ledcAttachPin(gpio, channel);
ledcWrite(channel, duty);
```

For newer Arduino-ESP32 versions, verify the current LEDC API because signatures changed across major versions.

## Deep Sleep

- Only RTC-capable GPIOs can wake from deep sleep.
- Variables in normal RAM are lost; use `RTC_DATA_ATTR` for retained values.
- Use `esp_sleep_enable_ext0_wakeup()` for one pin and `esp_sleep_enable_ext1_wakeup()` for multiple pins.
- WiFi reconnect after wake can take seconds; budget this in battery designs.
- Disable or power-gate external sensors if low sleep current matters.

## WiFi

- Call WiFi mode/setup before begin when using Arduino.
- Avoid relying only on `WiFi.setAutoReconnect(true)`; implement event-driven reconnect.
- Static IP can reduce connect time, but validate network requirements.
- WiFi TX current peaks can exceed 300 mA; weak power supplies cause brownout resets.

## FreeRTOS

- Give tasks enough stack. WiFi, printf, JSON, TLS, or camera code often needs much more than tiny examples.
- Yield inside loops with `vTaskDelay(pdMS_TO_TICKS(ms))`.
- Watchdog resets often mean a task is blocking too long or starving the idle task.
- On dual-core ESP32, WiFi/system work is commonly on core 0; pin application-heavy tasks carefully.
- Avoid sharing peripheral handles across tasks without synchronization.

## Peripherals

- I2C usually needs external pullups; internal pullups are weak.
- SPI CS must be controlled for each device.
- UART0 is often USB/programming console; use UART1/2 for external devices when possible.
- Camera, PSRAM, SD, and display pins are board-specific. Do not guess.

## OTA

- OTA requires partition support. Check partition table.
- Verify free sketch space before OTA.
- OTA handlers should run in the main loop or a responsive task.
- Plan rollback or recovery for field devices.

## Power

- Brownout resets usually indicate supply droop, not software.
- Add enough bulk capacitance near the module.
- Account for WiFi TX current peaks.
- For battery products, measure real sleep current, including regulators, pullups, sensors, and LEDs.

## Response Style

When answering ESP32 pin or peripheral questions:

- Name the chip variant and board assumption.
- List unsafe or risky pins.
- Explain ADC2/WiFi, boot strapping, flash pins, and input-only constraints when relevant.
- Prefer a pin table with reason for each choice.
