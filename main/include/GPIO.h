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
  LOW,
  HIGH
} GPIO_STATE;

typedef enum
{
  INPUT,
  OUTPUT
} GPIO_DIRECTION;

typedef struct
{
  uint8_t pin_num;           // ESP GPIO PIN NUM
  GPIO_DIRECTION direction;  // INPUT & OUTPUT
  GPIO_MODE mode;            // FOR OUTPUT
  GPIO_STATE state;          // FOR OUTPUT & LATCH&BLINK
  uint16_t blink_delay_ms;   // FOR OUTPUT & BLINK
  TickType_t last_wake_time; // FOR OUTPUT & BLINK
} pinMap_t;

#define MAX_PINS 8
extern pinMap_t pinmap[MAX_PINS];
extern int used_gpio_count;

/*STRUCT{pinname, mode, delay, state}*/

// public
void init_pinmap();
void blink_task();

// TODO: to make
bool set_gpio_direction(uint8_t pin, GPIO_DIRECTION dir);
bool set_gpio_mode(uint8_t pin, GPIO_DIRECTION mode);

bool set_gpio_state(uint8_t pin, uint16_t value);
bool set_gpio_blink(uint8_t pin, uint16_t value);

int digital_read(int pin);

#endif // GPIO_HEADER