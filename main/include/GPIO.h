#ifndef GPIO_HEADER
#define GPIO_HEADER

#include "driver/gpio.h"

// public
void handle_read_DI(int pin);

int digital_read(int pin);

#endif // GPIO_HEADER