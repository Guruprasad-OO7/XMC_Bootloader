#include "DAVE.h"

#define APP_VERSION "v2.3.1"

int main(void)
{
  DAVE_Init();

  /* USB CDC bring-up */
  USBD_VCOM_Connect();
  while (!USBD_VCOM_IsEnumDone());
  while (!cdc_event_flags.line_encoding_event_flag);
  for (volatile uint32_t i = 0; i < 50000000; i++);
  USBD_VCOM_SendString((int8_t *)"\r\n[APP ] Firmware " APP_VERSION "\r\n");
  USBD_VCOM_SendString((int8_t *)"[APP ] Build: " __DATE__ " " __TIME__ "\r\n");
  USBD_VCOM_SendString((int8_t *)"[APP ] USB VCOM initialized\r\n");
  USBD_VCOM_SendString((int8_t *)"[APP ] Application running\r\n");

  while (1)
  {
	 USBD_VCOM_SendString((int8_t *)"[APP ] Application running\r\n");
    CDC_Device_USBTask(&USBD_VCOM_cdc_interface);
    DIGITAL_IO_ToggleOutput(&LED);

    for (volatile uint32_t i = 0; i < 500000; i++);
  }
}
