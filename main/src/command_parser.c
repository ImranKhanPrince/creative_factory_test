

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "command_parser.h"
#include "uart1.h"
#include "uart0.h"
#include "GPIO.h"
#include "nvs.h"
#include "AO.h"
#include "AI.h"
// this file is architechture independant same as main.c

// Maximum tokens in a command (ex: "SET_DIO 1 BLINK 500 0xA5" → 5 tokens)
#define MAX_TOKENS 5
#define CHECKSUM_HEX_LEN 4 // "0xA5"

// STATIC SIGNATUERS

// TODO: move the handle functions to elsewhere
static int tokenize(char *buffer, char **tokens, const char *delim);
static uint8_t hex_to_uint8(const char *hex);
static void handle_uart_baud_change(int channel, int baud);
static void handle_pwm_value_change(int channel, float value);
static void handle_read_DI(int pin);
static void handle_read_ai(int channel);
static void handle_set_direction(uint8_t pin, char *direction);
static void handle_set_do(uint8_t pin, char *mode, uint16_t value);
static int get_nearest_baudrate(int requested_baudrate);

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
  for (const char *p = buffer; *p != '\0'; p++)
  {
    if (strncmp(p, "0x", 2) == 0)
    {
      break;
    }
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
  // SET_GPIO_DIR 2 OUTPUT Checksum (4)
  if (strcmp(tokens[0], "SET_GPIO_DIR") == 0)
  {
    uart1_log("CMD SET_GPIO_DIR\n");
    if (token_count != 4)
      return CMD_ERR_INVALID_FORMAT;
    cmd->type = CMD_SET_GPIO_DIRECTION;
    cmd->pin = atoi(tokens[1]); // pin no so that user can see the boards nuumber and enter that
    strncpy(cmd->mode, tokens[2], sizeof(cmd->mode));
  } // SET_DIO <Pin> <mode> <freq/state> <checksum>
  else if (strcmp(tokens[0], "SET_DO") == 0)
  {
    uart1_debug_print("SET_DO \n");
    if (token_count != 5)
      return CMD_ERR_INVALID_FORMAT;
    cmd->type = CMD_SET_DIO;
    cmd->pin = atoi(tokens[1]);
    strncpy(cmd->mode, tokens[2], sizeof(cmd->mode));
    cmd->value = atoi(tokens[3]); // Frequency or state
  }
  else if (strcmp(tokens[0], "READ_DI") == 0)
  {
    uart1_debug_print("READ_DI \n");
    if (token_count != 3)
      return CMD_ERR_INVALID_FORMAT;
    cmd->type = CMD_READ_DI;
    cmd->pin = atoi(tokens[1]); // pin number
  }
  else if (strcmp(tokens[0], "READ_AI") == 0)
  {
    uart1_debug_print("READ_AI \n");
    if (token_count != 3)
      return CMD_ERR_INVALID_FORMAT;
    cmd->type = CMD_READ_AI;
    cmd->channel = atoi(tokens[1]);
  }
  else if (strcmp(tokens[0], "SET_AO") == 0)
  {
    uart1_debug_print("SET_AO \n");
    if (token_count != 4)
      return CMD_ERR_INVALID_FORMAT;
    cmd->type = CMD_SET_AO;
    cmd->channel = atoi(tokens[1]);
    cmd->value = atof(tokens[2]);
  }
  else if (strcmp(tokens[0], "SET_UART_BAUD") == 0)
  {
    uart1_debug_print("SET_UART_BAUD \n");
    if (token_count != 4)
      return CMD_ERR_INVALID_FORMAT;
    cmd->type = CMD_SET_UART_BAUD;
    cmd->channel = atoi(tokens[1]); // uart channel
    cmd->value = atoi(tokens[2]);   // Baud rate
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
  case CMD_SET_GPIO_DIRECTION:
    handle_set_direction(cmd.pin, cmd.mode);
    break;

  case CMD_SET_DIO:
    handle_set_do(cmd.pin, cmd.mode, (uint16_t)cmd.value);
    break;

  case CMD_READ_AI:
    handle_read_ai(cmd.channel);
    break;

  case CMD_SET_AO:
    handle_pwm_value_change(cmd.channel, cmd.value);
    break;

  case CMD_READ_DI:
    handle_read_DI(cmd.pin);
    break;

  case CMD_SET_UART_BAUD:
    handle_uart_baud_change(cmd.channel, cmd.value);
    break;

  case CMD_ERROR:
    uart1_log("Unrecognized Command.\n");
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

void handle_uart_baud_change(int channel, int baud)
{
  char buf[50];
  sprintf(buf, "parsed baud %d\n", baud);
  uart1_debug_print(buf);
  if (channel == 1)
  {
    int nearest_baud = get_nearest_baudrate(baud);
    uart0_change_baudrate(nearest_baud);

    char buf[50];
    sprintf(buf, "nearest baud: %d\n", nearest_baud);
    uart1_debug_print(buf);

    uart0_baudrate_ = nearest_baud;
    save_uart_baudrates(); // nvs
  }
  else if (channel == 2)
  {
    int nearest_baud = get_nearest_baudrate(baud);
    uart1_change_baudrate(nearest_baud);
    char buf[50];
    sprintf(buf, "nearest baud: %d\n", nearest_baud);
    uart1_debug_print(buf);
    uart1_baudrate_ = nearest_baud;
    save_uart_baudrates(); // nvs
  }
  else
  {
    uart1_debug_print("invalid uart channel \n");
  }
}

static int get_nearest_baudrate(int requested_baudrate)
{
  const int valid_baudrates[] = {
      1200, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200,
      230400, 460800, 921600, 1500000, 2000000, 3000000};
  int num_baudrates = sizeof(valid_baudrates) / sizeof(valid_baudrates[0]); // 15*4/4

  int closest = valid_baudrates[0];
  int min_diff = abs(requested_baudrate - closest);

  for (int i = 1; i < num_baudrates; i++)
  {
    int diff = abs(requested_baudrate - valid_baudrates[i]);
    if (diff < min_diff)
    {
      min_diff = diff;
      closest = valid_baudrates[i];
    }
  }

  return closest;
}

static void handle_pwm_value_change(int channel, float value)
{
  set_pwm_voltage(channel, value);
  save_pwm_values(); // to nvs from the global variable
}

static void handle_read_ai(int channel)
{
  char buf[10];
  if (channel == 1)
  {
    int raw_value = read_adc_channel1();
    float voltage_value_V = (raw_value * ADC_VREF) / ADC_MAX;
    uart1_log("RAW Channel 1 Value: %d\n", raw_value);
    sprintf(buf, "%.2fV\n", voltage_value_V);
    uart0_print(buf);
  }
  else if (channel == 2)
  {
    int raw_value = read_adc_channel2();
    uart1_log("RAW Channel 2 Value: %d\n", raw_value);
    float voltage_value_V = (raw_value * ADC_VREF) / ADC_MAX;

    sprintf(buf, "%.2fV\n", voltage_value_V);
    uart0_print(buf);
  }
  else
  {
    uart1_log("Wrong ADC channel.\n");
  }
}

static void handle_read_DI(int pin)
{

  int pin_value = digital_read(pin);
  if (pin_value == 0)
  {
    uart0_print("LOW\n");
  }
  else if (pin_value == 1)
  {
    uart0_print("HIGH\n");
  }
  else if (pin_value == -1)
  {
    uart1_log("Pin is set to Output Mode\n");
  }
  else
  {
    uart1_log("Invalid pin value\n");
  }
}

static void handle_set_direction(uint8_t pin, char *direction)
{
  if (strcmp(direction, "INPUT") == 0)
  {
    if (!set_gpio_direction(pin, INPUT))
    {
      uart1_log("Failed to set direction for PIN %d\n", pin);
      return;
    }
    uart1_log("PIN:%d Direction set to OUTPUT\n", pin);
  }
  else if (strcmp(direction, "OUTPUT") == 0)
  {
    if (!set_gpio_direction(pin, OUTPUT))
    {
      uart1_log("Failed to set direction for PIN %d\n", pin);
      return;
    }
    uart1_log("PIN:%d Direction set to OUTPUT\n", pin);
  }
  else
  {
    uart1_log("Wrong Direction.\n");
  }
}

static void handle_set_do(uint8_t pin, char *mode, uint16_t value)
{
  if (strcmp(mode, "BLINK") == 0)
  {
    // Configure DIO pin to blink at cmd.value Hz
    set_gpio_blink(pin, value);
    // dio_set_blink(cmd.pin, cmd.value);
  }
  else if (strcmp(mode, "LATCH") == 0)
  {
    set_gpio_state(pin, value);
    // Set to INPUT/OUTPUT
    // dio_set_mode(cmd.pin, cmd.mode);
  }
  else
  {
    uart1_log("Invalid DO mode.");
  }
}