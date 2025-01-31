

#include "GPIO.h"
#include "uart0.h"
#include "uart1.h"
#include "nvs.h"

// in the delay state it will lopp throught the array and check pinMODE if BLINK then check delay and send to next state
pinMap_t pinmap[MAX_PINS] = {0};
int used_gpio_count = 0; // this will count the index of the array to insert the values of new gpio command TODO: obsolete
uint8_t pin_nums[8] = {21, 19, 18, 5, 17, 16, 4, 2};
// dev module pattern: [21, 19, 18, 5, 17, 16, 4, 2]
// sorted for easy understanding: [2, 4, 5, 16, 17, 18, 19, 21

static int8_t get_pinmap_index(uint8_t pin_num);

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
    pinmap[i].pin_num = pin_nums[i];
    pinmap[i].direction = OUTPUT;
    pinmap[i].mode = LATCH;
    pinmap[i].state = LOW;
    pinmap[i].blink_delay_ms = 0;
    pinmap[i].last_wake_time = xTaskGetTickCount();
  }
}
// TODO: a function named nvs load pinmap and that will also set the digital_out if latch mode

void blink_task()
{
  int i = 0;
  while (i < MAX_PINS)
  { // TODO: blink and OUTPUT Direction
    if (pinmap[i].direction == OUTPUT && pinmap[i].mode == BLINK)
    {
      const TickType_t delay_ticks = pdMS_TO_TICKS(pinmap[i].blink_delay_ms);
      TickType_t current_time = xTaskGetTickCount();

      if (current_time - pinmap[i].last_wake_time > delay_ticks)
      {
        uart1_log("Blinking pin: %d\n", pinmap[i].pin_num);
        gpio_set_direction(pinmap[i].pin_num, GPIO_MODE_OUTPUT);
        if (pinmap[i].state == HIGH)
        {
          gpio_set_level(pinmap[i].pin_num, 0);
          pinmap[i].state = LOW;
        }
        else if (pinmap[i].state == LOW)
        {
          gpio_set_level(pinmap[i].pin_num, 1);
          pinmap[i].state = HIGH;
        }
        pinmap[i].last_wake_time = current_time;
      }
    }
    ++i; // very important ++i
  }
}

static int8_t get_pinmap_index(uint8_t pin_num)
{
  int8_t pin_index = -1;
  for (uint8_t i = 0; i < MAX_PINS; i++)
  {
    if (pin_nums[i] == pin_num)
    {
      pin_index = i;
    }
  }
  return pin_index;
}

bool set_gpio_direction(uint8_t pin, GPIO_DIRECTION dir)
{
  if (dir == INPUT)
  {
    int8_t pin_index = get_pinmap_index(pin);
    uart1_log("pin index %d\n", pin_index);
    if ((int)pin_index < 0)
    {
      uart1_log("Wrong Pin Number\n");
      return false;
    }
    pinmap[pin_index].direction = dir;
    if (save_pinmap_to_nvs() != ESP_OK)
    {
      return false;
    }
    gpio_set_direction(pin, GPIO_MODE_INPUT);
    return true;
  }
  else if (dir == OUTPUT)
  {
    int8_t pin_index = get_pinmap_index(pin);
    uart1_log("pin index %d\n", pin_index);
    if (pin_index < 0)
    {
      uart1_log("Wrong Pin Number\n");
      return false;
    }
    pinmap[pin_index].direction = dir;
    if (save_pinmap_to_nvs() != ESP_OK)
    {
      return false;
    }
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    return true;
  }
  return false;
}

bool set_gpio_state(uint8_t pin, uint16_t value)
{
  int8_t pin_index = get_pinmap_index(pin);
  if (pin_index < 0)
  {
    uart1_log("Wrong Pin Number");
    return false;
  }
  else if (pinmap[pin_index].direction != OUTPUT)
  {
    uart1_log("Pin is set to Input Mode\n");
    return false;
  }
  else if (value != 1 && value != 0)
  {
    uart1_log("Wrong state.\n");
    return false;
  }
  pinmap[pin_index].mode = LATCH;
  pinmap[pin_index].state = (GPIO_STATE)value;
  if (save_pinmap_to_nvs() != ESP_OK)
  {
    return false;
  }
  gpio_set_direction(pin, GPIO_MODE_OUTPUT);
  gpio_set_level(pin, value);
  return true;
}
bool set_gpio_blink(uint8_t pin, uint16_t value)
{
  uint8_t pin_index = get_pinmap_index(pin);
  if (pin_index < 0)
  {
    uart1_log("Wrong Pin Number");
    return false;
  }
  else if (pinmap[pin_index].direction != OUTPUT)
  {
    uart1_log("Pin is set to Input Mode\n");
    return false;
  }
  else if (value < 1 && value > 1000)
  {
    uart1_log("Wrong Frequency.\n");
    return false;
  }
  pinmap[pin_index].mode = BLINK;
  pinmap[pin_index].blink_delay_ms = 1 * 1000 / value; // Hz to ms conversion

  if (save_pinmap_to_nvs() != ESP_OK)
  {
    return false;
  }
  return true;
}