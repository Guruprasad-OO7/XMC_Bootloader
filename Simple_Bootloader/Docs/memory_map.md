# XMC4800 Bootloader + USB CDC Application

A production-style two-stage firmware architecture for the Infineon XMC4800 MCU,
consisting of a custom bootloader and a USB CDC–enabled application with a safe
Cortex-M handover.

---

## ✨ Features

### Bootloader
- Runs from internal flash at `0x08000000`
- Timed boot decision window (button-based)
- Optional bootloader hold mode
- Deterministic startup (no USB dependency by default)
- Safe Cortex-M jump:
  - SysTick stopped
  - NVIC interrupts cleared
  - VTOR relocated
  - MSP switched
  - Jump via application reset handler

### Application
- Runs from `0x08010000`
- USB CDC (USBD_VCOM) communication
- Enumeration-aware logging
- Firmware version and build info reporting

---

## 🧠 Why this project

Most examples stop at LED blinking or treat USB like UART.
This project focuses on **correct bootloader design**, **USB lifecycle handling**,
and **safe context transfer** between firmware stages.

It addresses real-world issues such as:
- USB enumeration timing
- Interrupt leakage across firmware boundaries
- Silent failures during MSP/VTOR handover

---

## 🗺 Memory Layout

| Region        | Address        | Purpose        |
|---------------|----------------|----------------|
| Bootloader    | 0x08000000     | First-stage FW |
| Application   | 0x08010000     | Main FW       |

---

## 🔁 Boot Flow

1. Reset
2. Bootloader starts
3. LED slow blink during timeout window
4. Button pressed?
   - YES → stay in bootloader (USB CDC active)
   - NO  → jump to application
5. Application initializes USB CDC and runs

---

## 📟 Example Output

### Bootloader (hold mode)


---

## 📍 RAM Usage

Both bootloader and application use internal SRAM.

### Bootloader
- Uses MSP during startup
- SysTick enabled only during decision window
- All interrupts disabled before handover

### Application
- Re-initializes MSP using its own vector table
- Owns SysTick, NVIC, and peripherals
- Independent runtime environment

---

## 🧷 Vector Tables

| Firmware     | Vector Table Location |
|--------------|----------------------|
| Bootloader   | `0x08000000` |
| Application  | `0x08010000` |

Before jumping to the application, the bootloader:
- Relocates `SCB->VTOR` to `0x08010000`
- Loads MSP from the application vector table

---

## ⚠️ Important Notes

- Bootloader **must be flashed before** the application
- Flashing the bootloader after the application may overwrite it
- Application linker script **must** place the vector table at `0x08010000`

---

## ✅ Summary

This memory layout enables:
- Safe firmware upgrades
- Deterministic boot behavior
- Clean separation of responsibilities
- Scalable extension (CRC, dual-image, OTA, etc.)