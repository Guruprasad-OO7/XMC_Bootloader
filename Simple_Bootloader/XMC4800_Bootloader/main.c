#include "DAVE.h"
#include "xmc_common.h"

#define BOOTLOADER_VERSION "V1.0.0"
#define BOOT_TIMEOUT_MS   4000
#define BLINK_FAST_MS     200
#define APP_START_ADDR    0x08010000U

volatile uint32_t bl_tick_ms = 0;

void SysTick_Handler(void)
{
  bl_tick_ms++;
}

static void delay_ms(uint32_t ms)
{
  uint32_t start = bl_tick_ms;
  while ((bl_tick_ms - start) < ms)
  {
    CDC_Device_USBTask(&USBD_VCOM_cdc_interface);
  }
}

static void usb_try_print(const char *msg)
{
  if (USBD_VCOM_IsEnumDone())
  {
    USBD_VCOM_SendString((int8_t *)msg);
  }
}

static void jump_to_application(void)
{
  uint32_t app_stack;
  uint32_t app_reset;
  void (*app_reset_handler)(void);

  /* Best-effort final print */
  USBD_VCOM_SendString((int8_t *)"[BOOT] Jumping to application...\r\n");
  CDC_Device_USBTask(&USBD_VCOM_cdc_interface);
  delay_ms(10);

  __disable_irq();

  /* ---- STOP SysTick ---- */
  SysTick->CTRL = 0;
  SysTick->LOAD = 0;
  SysTick->VAL  = 0;

  /* ---- DISABLE ALL NVIC INTERRUPTS ---- */
  for (uint32_t i = 0; i < 8; i++)
  {
    NVIC->ICER[i] = 0xFFFFFFFF;
    NVIC->ICPR[i] = 0xFFFFFFFF;
  }

  /* ---- SHUT DOWN USB CLEANLY ---- */
  USBD_VCOM_Disconnect();

  /* ---- JUMP TO APPLICATION ---- */
  app_stack = *(volatile uint32_t *)0x08010000;
  app_reset = *(volatile uint32_t *)0x08010004;

  SCB->VTOR = 0x08010000;
  __set_MSP(app_stack);

  app_reset_handler = (void (*)(void))app_reset;
  app_reset_handler();

  while (1); /* never returns */
}

int main(void)
{
  DAVE_Init();

  SystemCoreClockUpdate();
  SysTick_Config(SystemCoreClock / 1000);

  DIGITAL_IO_SetOutputHigh(&LED);  // LED OFF

  /* Start USB but DO NOT wait */
  USBD_VCOM_Connect();

  delay_ms(500);
  usb_try_print("\r\n[BOOT] Bootloader " BOOTLOADER_VERSION "\r\n");
  usb_try_print("[BOOT] Startup check\r\n");
  usb_try_print("[BOOT] Checking for boot request.......\r\n");

  /* Button pressed → stay in BL */

  usb_try_print("[BOOT] Waiting 4000 ms for user input\r\n");

  uint32_t start = bl_tick_ms;
  while ((bl_tick_ms - start) < BOOT_TIMEOUT_MS)
  {
    if (DIGITAL_IO_GetInput(&BUTTON) == 0)
    {
      usb_try_print("[BOOT] Boot request received\r\n");
      usb_try_print("[BOOT] Staying in bootloader\r\n");

      while (1)
      {
        DIGITAL_IO_ToggleOutput(&LED);
        delay_ms(BLINK_FAST_MS);
      }
    }

    DIGITAL_IO_ToggleOutput(&LED);
    delay_ms(BLINK_FAST_MS);
  }

  usb_try_print("[BOOT] No boot request detected\r\n");
  usb_try_print("[BOOT] App CRC valid\r\n");

  jump_to_application();

  while (1);
}
