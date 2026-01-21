# Bootloader

Custom first-stage bootloader for XMC4800.

## Responsibilities
- Startup decision logic
- Optional USB CDC in hold mode
- Safe handover to application

## Does NOT
- Depend on USB during normal boot
- Print before USB enumeration
- Leave interrupts enabled before jump

## Bootloader → Application Handover

The bootloader performs a clean Cortex-M handover by:
1. Disabling SysTick
2. Clearing NVIC enable & pending registers
3. Relocating the vector table (VTOR)
4. Switching the main stack pointer (MSP)
5. Jumping via the application reset vector

This avoids undefined behavior and hard-to-debug faults.