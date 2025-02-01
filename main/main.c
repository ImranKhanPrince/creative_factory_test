#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "uart0.h"
#include "uart1.h"
#include "command_parser.h"
#include "GPIO.h"
#include "nvs.h"
#include "AO.h"
#include "AI.h"

#define DELAY_TIME_MS 1000

typedef enum
{
  STATE_COMMAND,
  STATE_BLINK,
} state_t;

void app_main(void)
{

  init_nvs();            // first peripheral to open
  load_uart_baudrates(); // from nvs to extern variable

  uart0_init(); // load baudrate first
  uart1_init(); // needs for debug print

  load_pwm_values(); // from nvs to extern variable
  init_pinmap();
  start_pwm_channel1();
  start_pwm_channel2();
  init_adc_channels();
  // cleanup_adc(); // No need

  if (load_pinmap_from_nvs() != ESP_OK)
  {
    uart1_log("Failed to Load the pinmap value from nvs\n");
  }

  state_t state = STATE_COMMAND;

  while (true)
  {
    switch (state)
    {
    case STATE_COMMAND:
      char buf[100];

      if (uart0_read_newline(buf))
      {
        process_uart_command(buf);
        uart1_debug_print(buf);
      }
      state = STATE_BLINK;
      break;

    case STATE_BLINK:
      blink_task();
      state = STATE_COMMAND;
      break;

    default:
      break;
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
