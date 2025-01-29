#ifndef UART1_HEADER
#define UART1_HEADER

#include "string.h"
#include "driver/uart.h"
#include "esp_err.h"

#define UART1_TX_PIN 25 // Default ESP32 UART0 TX (change if needed)
#define UART1_RX_PIN 23 // Default ESP32 UART0 RX (change if needed)
#define UART1_BUF_SIZE 1024

#define UART1_LOG_BUFFER_SIZE 256
#define uart1_log(format, ...)                                       \
  do                                                                 \
  {                                                                  \
    char log_buf[UART1_LOG_BUFFER_SIZE];                             \
    snprintf(log_buf, UART1_LOG_BUFFER_SIZE, format, ##__VA_ARGS__); \
    uart1_debug_print(log_buf);                                      \
  } while (0)

extern int uart1_baudrate_;

void uart1_init();
void uart1_debug_print(char *buf);
void uart1_change_baudrate(int new_baudrate);

#endif // UART1_HEADER