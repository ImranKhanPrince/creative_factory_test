#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <stdint.h>
#include <stdbool.h>

// Command types (expand as needed)
typedef enum
{
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
  uint32_t value;   // Frequency, AO value, or baud rate
  char mode[10];    // "INPUT", "OUTPUT", "BLINK"
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
CommandError parse_command(const char *buffer, ParsedCommand *cmd);
bool validate_checksum(const char *buffer, uint8_t received_checksum);
void process_uart_command(char *buffer);

// private
void print_command_error(CommandError err);

#endif // COMMAND_PARSER_H
