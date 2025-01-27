#include "uart1.h"
// TODO: change baudrate
void uart1_init()
{
  // Configure UART parameters
  // Configure UART parameters
  uart_config_t uart_config = {
      .baud_rate = UART1_BAUD_RATE,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .source_clk = UART_SCLK_DEFAULT};

  // Install UART driver
  ESP_ERROR_CHECK(uart_driver_install(
      UART_NUM_1,         // UART port number
      UART1_BUF_SIZE * 2, // RX buffer size
      UART1_BUF_SIZE * 2, // TX buffer size
      0,                  // Event queue size (0 if not used)
      NULL,               // Event queue handle (NULL if not used)
      0                   // Flags (none)
      ));

  // Configure UART parameters
  ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &uart_config));

  // Set UART pins
  ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, UART1_TX_PIN, UART1_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
}

void uart1_debug_print(char *buf)
{
  if (buf != NULL)
  {
    uart_write_bytes(UART_NUM_1, buf, strlen(buf));
  }
}