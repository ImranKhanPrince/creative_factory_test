#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <stdint.h>
#include <stdbool.h>

// Command types (expand as needed)
typedef enum
{
  CMD_SET_GPIO_DIRECTION,
  CMD_READ_AI,       // Format: "READ_AI <channel> <checksum>"
  CMD_SET_AO,        // Format: "SET_AO <channel> <value> <checksum>"
  CMD_SET_DIO,       // Format: "SET_DIO <pin> <mode> <freq> <checksum>"
  CMD_READ_DI,       // Format: "READ_DI" <pin> <checksum>
  CMD_SET_UART_BAUD, // Format: "SET_UART_BAUD <uart_no> <baud> <checksum>"
  CMD_ERROR          // Invalid command
} CommandType;

// Parsed command data
typedef struct
{
  CommandType type; // TYPE OF COMMAND
  uint8_t pin;      // For DIO commands
  uint8_t channel;  // For AI/AO commands AND UART CHANNEL
  float value;      // Frequency or state, AO value, or baud rate
  char mode[10];    // "INPUT", "OUTPUT", "BLINK", "LATCH"
} ParsedCommand;

// Error codes
typedef enum
{
  CMD_ERR_NONE,
  CMD_ERR_INVALID_FORMAT,
  CMD_ERR_CHECKSUM_MISMATCH,
  CMD_ERR_INVALID_PARAM
} CommandError;

// Public functions

void process_uart_command(char *buffer);

// private

#endif // COMMAND_PARSER_H
