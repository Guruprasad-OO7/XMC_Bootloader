# Boot Flow

This document explains the complete boot sequence from reset to application
execution, including decision logic and Cortex-M handover steps.

---

## 🔁 High-Level Boot Sequence

1. MCU Reset
2. Bootloader execution
3. Boot decision window
4. Optional bootloader hold mode
5. Clean jump to application
6. Application startup and USB CDC initialization

---

## 🔌 Stage 1: Bootloader

### Entry Point
- Execution begins at `0x08000000`
- Bootloader reset handler runs
- Vector table points to bootloader vectors

---

### Boot Decision Window

After reset, the bootloader:

- Starts a **timer-based decision window** (~4 seconds)
- Uses LED slow blinking as state indication
- Monitors a user button

#### Decision Logic

| Condition | Action |
|----------|--------|
| Button pressed | Stay in bootloader |
| No input | Jump to application |

---

### Bootloader Hold Mode

When a boot request is detected:

- Bootloader stays active indefinitely
- USB CDC is enabled
- Status messages are printed over USB
- LED blinks fast to indicate hold mode

This mode is intended for:
- Firmware update
- Debug access
- Recovery scenarios

---

## 🔄 Stage 2: Bootloader → Application Handover

To safely transfer control, the bootloader performs a **clean Cortex-M context switch**.

### Required Steps

1. Disable global interrupts
2. Stop SysTick completely
3. Clear all NVIC enable and pending registers
4. Read application vector table:
   - Initial MSP
   - Reset handler address
5. Relocate vector table (`SCB->VTOR`)
6. Load MSP
7. Jump to application reset handler

---

### Why This Is Important

Skipping any of these steps can cause:
- HardFaults
- Silent crashes
- Random behavior
- USB or interrupt instability

This handover ensures the application starts in a **clean and predictable state**.

---

## 🚀 Stage 3: Application

Once control is transferred:

- Application reset handler runs
- Application initializes clocks and peripherals
- USB CDC is initialized and enumerated
- Firmware identity is printed
- Main application loop executes

---

## 📟 Observable Behavior

### Normal Boot (No Button Pressed)

- LED slow blink → fast blink
- Application USB CDC enumerates
- Application messages appear

---

### Bootloader Hold Mode

- Immediate fast LED blink
- USB CDC enumerates
- Bootloader messages appear
- Application is not started

---

## 🧠 Design Philosophy

- Bootloader is **minimal and deterministic**
- USB is **optional**, not mandatory
- Application owns runtime resources
- Clear separation between stages
- No shared global state

---

## ✅ Summary

This boot flow ensures:
- Reliable startup
- Safe firmware transitions
- Predictable USB behavior
- Production-grade robustness