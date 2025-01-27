
#include "GPIO.h"
#include "uart0.h"
#include "uart1.h"

// TODO: make a array of gpio matrix
/*STRUCT{pinname, mode, delay, state}*/
/*Array[struct_pin]
in the delay state it will lopp throught the array and check pinMODE if BLINK then check delay and send to next state
*/

int digital_read(int pin)
{
  // Reset the pin to its default state
  gpio_reset_pin(pin);

  // Configure the pin as input
  gpio_set_direction(pin, GPIO_MODE_INPUT);

  // Read the pin value
  int value = gpio_get_level(pin);

  return value;
}

void handle_read_DI(int pin)
{
  // TODO: make a error handler if gpio number is invalid.

  int pin_value = digital_read(pin);
  if (pin_value == 0)
  {
    uart0_print("LOW\n");
  }
  else if (pin_value == 1)
  {
    uart0_print("HIGH\n");
  }
  else
  {
    uart1_debug_print("Invalid pin value\n");
  }
}