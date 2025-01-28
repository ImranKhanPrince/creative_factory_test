#ifndef UART1_HEADER
#define UART1_HEADER

#include "string.h"
#include "driver/uart.h"
#include "esp_err.h"

#define UART1_TX_PIN 25 // Default ESP32 UART0 TX (change if needed)
#define UART1_RX_PIN 23 // Default ESP32 UART0 RX (change if needed)
#define UART1_BUF_SIZE 1024
#define UART1_BAUD_RATE 115200

extern int uart1_baudrate_;

void uart1_init();
void uart1_debug_print(char *buf);

#endif // UART1_HEADER