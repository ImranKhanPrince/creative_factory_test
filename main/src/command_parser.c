

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "command_parser.h"
#include "uart1.h"
#include "GPIO.h"

// Maximum tokens in a command (e.g., "SET_DIO 1 BLINK 500 0xA5" → 5 tokens)
#define MAX_TOKENS 5
#define CHECKSUM_HEX_LEN 4 // "0xA5"

static int tokenize(char *buffer, char **tokens, const char *delim);
static uint8_t hex_to_uint8(const char *hex);

// Tokenize the command into an array of strings
static int tokenize(char *buffer, char **tokens, const char *delim)
{
  int count = 0;
  char *token = strtok(buffer, delim);
  while (token != NULL && count < MAX_TOKENS)
  {
    tokens[count++] = token;
    token = strtok(NULL, delim);
  }
  return count;
}

// Convert hex string to uint8 (for checksum)
static uint8_t hex_to_uint8(const char *hex)
{
  return (uint8_t)strtol(hex, NULL, 16);
}

// Validate checksum (XOR of all bytes before checksum)
bool validate_checksum(const char *buffer, uint8_t received_checksum)
{
  uint8_t calculated = 0;
  for (const char *p = buffer; *p != '\0' && *p != '0'; p++)
  { // Stop before "0x"
    calculated ^= *p;
  }
  return (calculated == received_checksum);
}

// NOTE: if back to back 2 new line comes then causes one command to faile
// NOTE: if the data is at typing speed then parser will parse erorr as no buffer is used to fill up the comand

CommandError parse_command(const char *buffer, ParsedCommand *cmd)
{
  char buf_copy[128]; // Static buffer (adjust size as needed)
  strncpy(buf_copy, buffer, sizeof(buf_copy));
  buf_copy[sizeof(buf_copy) - 1] = '\0'; // Ensure null-termination

  char *tokens[MAX_TOKENS];
  int token_count = tokenize(buf_copy, tokens, " \n");

  // Minimum tokens: command + checksum (e.g., "READ_AI 0 0x3F" → 3 tokens)
  if (token_count < 2)
    return CMD_ERR_INVALID_FORMAT;

  // Extract checksum (last token)
  char *checksum_str = tokens[token_count - 1];
  if (strncmp(checksum_str, "0x", 2) != 0)
    return CMD_ERR_INVALID_FORMAT;
  uint8_t checksum = hex_to_uint8(checksum_str + 2); // Skip "0x"

  // Validate checksum (exclude checksum itself from calculation)
  char *cmd_part = strstr(buffer, checksum_str) - 1; // Find start of checksum
  *cmd_part = '\0';                                  // Temporarily truncate buffer
  bool valid = validate_checksum(buffer, checksum);
  *cmd_part = ' '; // Restore buffer (optional)
  if (!valid)
    return CMD_ERR_CHECKSUM_MISMATCH;

  // Parse command type
  if (strcmp(tokens[0], "SET_DIO") == 0)
  {
    if (token_count != 5)
      return CMD_ERR_INVALID_FORMAT;
    cmd->type = CMD_SET_DIO;
    cmd->pin = atoi(tokens[1]);
    strncpy(cmd->mode, tokens[2], sizeof(cmd->mode));
    cmd->value = atoi(tokens[3]); // Frequency
  }
  else if (strcmp(tokens[0], "READ_DI") == 0)
  {
    if (token_count != 3)
      return CMD_ERR_INVALID_FORMAT;
    cmd->type = CMD_READ_DI;
    cmd->pin = atoi(tokens[1]); // pin number
  }
  else if (strcmp(tokens[0], "READ_AI") == 0)
  {
    if (token_count != 3)
      return CMD_ERR_INVALID_FORMAT;
    cmd->type = CMD_READ_AI;
    cmd->pin = atoi(tokens[1]);
  }
  else if (strcmp(tokens[0], "SET_AO") == 0)
  {
    if (token_count != 4)
      return CMD_ERR_INVALID_FORMAT;
    cmd->type = CMD_SET_AO;
    cmd->channel = atoi(tokens[1]);
    cmd->value = atoi(tokens[2]);
  }
  else if (strcmp(tokens[0], "SET_UART_BAUD") == 0)
  {
    if (token_count != 3)
      return CMD_ERR_INVALID_FORMAT;
    cmd->type = CMD_SET_UART_BAUD;
    cmd->value = atoi(tokens[1]); // Baud rate
  }
  else
  {
    cmd->type = CMD_ERROR;
    return CMD_ERR_INVALID_PARAM;
  }

  return CMD_ERR_NONE;
}

void process_uart_command(char *buffer)
{
  ParsedCommand cmd = {0};
  CommandError err = parse_command(buffer, &cmd);

  if (err != CMD_ERR_NONE)
  {
    print_command_error(err);
    return;
  }

  switch (cmd.type)
  {
    // TODO: implement these functionality
  case CMD_SET_DIO:
    if (strcmp(cmd.mode, "BLINK") == 0)
    {
      // Configure DIO pin to blink at cmd.value Hz
      // dio_set_blink(cmd.pin, cmd.value);
    }
    else
    {
      // Set to INPUT/OUTPUT
      // dio_set_mode(cmd.pin, cmd.mode);
    }
    break;

  case CMD_READ_AI:
    // uint16_t value = analog_read(cmd.channel);
    // printf("AI%d: %d\n", cmd.channel, value);
    break;

  case CMD_SET_AO:
    // analog_write(cmd.channel, cmd.value);
    break;

  case CMD_READ_DI:
    handle_read_DI(cmd.pin);
    break;

  case CMD_SET_UART_BAUD:
    // uart_set_baudrate(cmd.value);
    break;

  case CMD_ERROR:
    // log error message
    break;

  default:
    break;
  }
}

void print_command_error(CommandError err)
{
  switch (err)
  {
  case CMD_ERR_NONE:
    uart1_debug_print("No Error\n");
    break;
  case CMD_ERR_INVALID_FORMAT:
    uart1_debug_print("Invalid Format\n");
    break;
  case CMD_ERR_CHECKSUM_MISMATCH:
    uart1_debug_print("Checksum Mismatch\n");
    break;
  case CMD_ERR_INVALID_PARAM:
    uart1_debug_print("Invalid Parameter\n");
    break;
  default:
    uart1_debug_print("Unknown Error\n");
    break;
  }
}