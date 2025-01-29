

#include "GPIO.h"
#include "uart0.h"
#include "uart1.h"

// in the delay state it will lopp throught the array and check pinMODE if BLINK then check delay and send to next state
pinMap_t pinmap[MAX_PINS] = {0};
int used_gpio_count = 0;                    // this will count the index of the array to insert the values of new gpio command
int pin_nums[8] = {1, 2, 3, 4, 5, 6, 7, 8}; // TODO: use original usable pin no here

int digital_read(int pin)
{
  // TODO: check the pin number from pin_nums then check if DIRECTION is output if not then give error but read the value.

  // Reset the pin to its default state
  gpio_reset_pin(pin);

  // Configure the pin as input
  gpio_set_direction(pin, GPIO_MODE_INPUT);

  // Read the pin value
  int value = gpio_get_level(pin);

  return value;
}

void init_pinmap()
{
  for (int i = 0; i < MAX_PINS; i++)
  {
    // TODO: make the pins by default outward and configure the
    pinmap[i].mode = LATCH;
    pinmap[i].blink_delay_ms = 0;
    pinmap[i].pin_name = GPIO_NUM_NC;
    pinmap[i].state = LOW;
    pinmap[i].last_wake_time = xTaskGetTickCount();
  }
}
// TODO: a function named nvs load pinmap and that will also set the digital_out if latch mode

void blink_task()
{
  int i = 0;
  while (pinmap[i].pin_name != GPIO_NUM_NC)
  { // TODO: blink and OUTPUT Direction
    if (pinmap[i].mode == BLINK)
    {
      const TickType_t delay_ticks = pdMS_TO_TICKS(pinmap[i].blink_delay_ms);
      TickType_t current_time = xTaskGetTickCount();

      if (current_time - pinmap[i].last_wake_time > delay_ticks)
      {
        char buf[32];
        sprintf(buf, "Blinking pin: %d\n", pinmap[i].pin_name);
        uart1_debug_print(buf);

        gpio_set_direction(pinmap[i].pin_name, GPIO_MODE_OUTPUT);
        if (pinmap[i].state == HIGH)
        {
          gpio_set_level(pinmap[i].pin_name, 0);
          pinmap[i].state = LOW;
        }
        else if (pinmap[i].state == LOW)
        {
          gpio_set_level(pinmap[i].pin_name, 1);
          pinmap[i].state = HIGH;
        }
        pinmap[i].last_wake_time = current_time;
      }
    }
    ++i; // very important ++i
  }
}