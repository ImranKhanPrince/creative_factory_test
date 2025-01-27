#ifndef GPIO_HEADER
#define GPIO_HEADER

#include "driver/gpio.h"

// public
void handle_read_DI(int pin);

// TODO: to make
void dio_set_blink(int pin, int value);
void dio_set_mode(int pin, char *mode);

int digital_read(int pin);

#endif // GPIO_HEADER