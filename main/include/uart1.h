#ifndef UART2_HEADER
#define UART2_HEADER

#include "string.h"
#include "driver/uart.h"
#include "esp_err.h"

#define UART1_TX_PIN 25 // Default ESP32 UART0 TX (change if needed)
#define UART1_RX_PIN 23 // Default ESP32 UART0 RX (change if needed)
#define UART1_BUF_SIZE 1024
#define UART1_BAUD_RATE 115200
#define PATTERN_CHR_NUM 1 // Number of characters to detect ('\n')

void uart1_init();
void uart1_debug_print(char *buf);

#endif // UART2_HEADER