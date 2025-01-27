#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "uart0.h"
#include "uart1.h"
#include "command_parser.h"

#define DELAY_TIME_MS 1000

typedef enum
{
  STATE_IDLE,
  STATE_DELAY,
  STATE_WRITE,
  STATE_ACTION
} state_t;

void app_main(void)
{
  TickType_t last_wake_time = xTaskGetTickCount();
  const TickType_t delay_ticks = pdMS_TO_TICKS(DELAY_TIME_MS);

  uart0_init();
  uart1_init();

  state_t state = STATE_IDLE;

  while (true)
  {
    switch (state)
    {
    case STATE_IDLE:
      char buf[100]; // fixed sized buffer but command will end with \n so strlen() can measure the size

      if (uart0_read_newline(buf))
      {
        process_uart_command(buf);
        uart1_debug_print(buf);
      }
      // uart0_print("Sate IDLE\n");
      /* code */
      state = STATE_DELAY;
      break;

    case STATE_DELAY:
      // uart0_print("State DELAY\n");
      // TODO: this state will blink the led. when time passes it will send to toggle led state.
      if (xTaskGetTickCount() - last_wake_time >= delay_ticks)
      {
        last_wake_time = xTaskGetTickCount();
        state = STATE_WRITE;
        break;
      }
      /* code */

      state = STATE_IDLE;
      break;

    case STATE_WRITE:
      // uart0_print("Sate WRITE\n");
      uart0_print("Hello each second\n");
      state = STATE_IDLE;
      break;

    default:
      break;
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
