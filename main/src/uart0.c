#include "string.h"

#include "driver/uart.h"
#include "esp_err.h"

#include "uart0.h"

int uart0_baudrate_ = 115200;
// TODO: change baudrate

void uart0_init()
{
  // Configure UART parameters
  // Configure UART parameters
  uart_config_t uart_config = {
      .baud_rate = uart0_baudrate_,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .source_clk = UART_SCLK_DEFAULT};

  // Install UART driver
  ESP_ERROR_CHECK(uart_driver_install(
      UART_NUM_0,         // UART port number
      UART0_BUF_SIZE * 2, // RX buffer size
      UART0_BUF_SIZE * 2, // TX buffer size
      0,                  // Event queue size (0 if not used)
      NULL,               // Event queue handle (NULL if not used)
      0                   // Flags (none)
      ));

  // Configure UART parameters
  ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &uart_config));

  // Set UART pins
  ESP_ERROR_CHECK(uart_set_pin(UART_NUM_0, UART0_TX_PIN, UART0_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
}

void uart0_print(char *buf)
{
  if (buf != NULL)
  {
    uart_write_bytes(UART_NUM_0, buf, strlen(buf));
  }
}

bool uart0_read_newline(char *buf)
{
  int index = 0;

  // Read one byte from UART
  while (uart_read_bytes(UART_NUM_0, (uint8_t *)&buf[index], 1, 5 / portTICK_PERIOD_MS) > 0)
  {
    // Check if the character is a newline
    if (buf[index] == '\n')
    {
      buf[index + 1] = '\0'; // Null-terminate the string
      return true;
    }
    index++;
  }

  buf[index] = '\0'; // Null-terminate the string if buffer is full
  return false;
}