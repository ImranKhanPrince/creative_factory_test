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

  init_nvs(); // first peripheral to open
  // uart1_baudrate_ = 115200;
  // save_uart_baudrates();
  load_uart_baudrates(); // from nvs to extern variable

  uart0_init(); // load baudrate first
  uart1_init(); // needs for debug print

  load_pwm_values(); // from nvs to extern variable
  init_pinmap();
  start_pwm_channel1();
  start_pwm_channel2();
  init_adc_channels();
  // cleanup_adc(); // No need

  // TODO: Pinmap and nvs needs heavily modification. used_gpio_count obsolete and pin name is pin_num
  if (load_pinmap_from_nvs() != ESP_OK)
  {
    uart1_log("Failed to Load the pinmap value from nvs\n");
  }

  // TickType_t last_wake_time = xTaskGetTickCount();
  // const TickType_t delay_ticks = pdMS_TO_TICKS(DELAY_TIME_MS);
  // pinmap[0].mode = BLINK;
  // pinmap[0].last_wake_time = xTaskGetTickCount();
  // pinmap[0].blink_delay_ms = 1000;
  // pinmap[0].state = LOW;
  // pinmap[0].pin_name = (gpio_num_t)2;
  // ++used_gpio_count;
  // if (save_pinmap_to_nvs() != ESP_OK)
  // {
  //   uart1_debug_print("Failed to SAVE the pinmap value from nvs\n");
  // }

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
      // uart0_print("Sate IDLE\n");
      /* code */
      state = STATE_BLINK;
      break;

    case STATE_BLINK:
      // uart0_print("State DELAY\n");
      // TODO: this state will blink the led. when time passes it will send to toggle led state.

      /* code */
      // typically if something then goes to a different case.
      blink_task();
      state = STATE_COMMAND;
      break;

    default:
      break;
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
