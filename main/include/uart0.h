#ifndef UART0_H
#define UART0_H

#include "stdbool.h"

#define UART0_TX_PIN 1 // Default ESP32 UART0 TX (change if needed)
#define UART0_RX_PIN 3 // Default ESP32 UART0 RX (change if needed)
#define UART0_BUF_SIZE 1024
#define UART0_BAUD_RATE 115200

void uart0_init();
void uart0_print(char *buf);
bool uart0_read_newline(char *buf);

#endif // UART0_H