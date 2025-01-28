#ifndef GPIO_HEADER
#define GPIO_HEADER

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

typedef enum
{
  BLINK,
  LATCH
} GPO_MODE;
typedef enum
{
  HIGH,
  LOW
} GPO_STATE;

typedef struct
{
  int pin_name;
  GPO_MODE mode;
  GPO_STATE state;
  int blink_delay_ms;
  gpio_num_t last_wake_time;
} pinMap_t;

#define MAX_PINS 8
extern pinMap_t pinmap[MAX_PINS];
extern int used_gpio_count;

/*STRUCT{pinname, mode, delay, state}*/

// public
void handle_read_DI(int pin);
void init_pinmap();
void blink_task();

// TODO: to make
void dio_set_blink(int pin, int value);
void dio_set_mode(int pin, char *mode);

int digital_read(int pin);

#endif // GPIO_HEADER