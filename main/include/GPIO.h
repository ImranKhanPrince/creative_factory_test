#ifndef GPIO_HEADER
#define GPIO_HEADER

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

typedef enum
{
  BLINK,
  LATCH
} GPIO_MODE;
typedef enum
{
  HIGH,
  LOW
} GPIO_STATE;

typedef enum
{
  INPUT,
  OUTPUT
} GPIO_DIRECTION;

// typedef struct
// {
//   uint8_t index;
//   uint8_t pin_name;          // pin_number of esp
//   GPIO_DIRECTION direction;  // INPUT OUTPUT DIRECTION
//   GPIO_MODE mode;            // FOR OUTPUT DIRECTION
//   GPIO_STATE state;          // FOR OUTPUT DIRECTION
//   int blink_delay_ms;        // FOR OUTPUT DIRECTION
//   gpio_num_t last_wake_time; // FOR OUTPUT DIRECTION
// } pinMap_t;

typedef struct
{
  int pin_name; // TODO: this will no longer come from nvs insted it will come from hardcoded array
  GPIO_MODE mode;
  GPIO_STATE state;
  int blink_delay_ms;
  gpio_num_t last_wake_time; // TODO: why gpio_num_t somethng is very wrong figure out
} pinMap_t;

#define MAX_PINS 8
extern pinMap_t pinmap[MAX_PINS];
extern int used_gpio_count;

/*STRUCT{pinname, mode, delay, state}*/

// public
void init_pinmap();
void blink_task();

// TODO: to make
void do_set_blink(int pin, int value);
void do_set_mode(int pin, char *mode);

int digital_read(int pin);

#endif // GPIO_HEADER