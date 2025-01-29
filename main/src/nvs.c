#include "nvs.h"
#include "uart1.h"
#include "uart0.h"
#include "gpio.h"
#include "AO.h"

// TODO: uart baudrate, analog_IO values

esp_err_t init_nvs(void)
{
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  return err;
}
// TODO: adjust the pinmap according to new addition
esp_err_t save_pinmap_to_nvs(void)
{
  nvs_handle_t nvs_handle;
  esp_err_t err;
  char debug_buf[64];

  // Open NVS debug
  uart1_debug_print("Opening NVS...\n");
  err = nvs_open("gpio_config", NVS_READWRITE, &nvs_handle);
  if (err != ESP_OK)
  {
    sprintf(debug_buf, "NVS open failed! Error: %d\n", err);
    uart1_debug_print(debug_buf);
    return err;
  }

  // Save count debug
  sprintf(debug_buf, "Saving used_count: %d\n", used_gpio_count);
  uart1_debug_print(debug_buf);

  err = nvs_set_u8(nvs_handle, "used_count", used_gpio_count);
  if (err != ESP_OK)
  {
    sprintf(debug_buf, "Failed to save used_count! Error: %d\n", err);
    uart1_debug_print(debug_buf);
    nvs_close(nvs_handle);
    return err;
  }

  // Save pins debug
  for (int i = 0; i < used_gpio_count; i++)
  {
    char key[16];

    sprintf(debug_buf, "Saving pin %d config...\n", i);
    uart1_debug_print(debug_buf);

    // Save pin number
    sprintf(key, "pin%d_num", i);
    sprintf(debug_buf, "Key: %s, Value: %d\n", key, pinmap[i].pin_name);
    uart1_debug_print(debug_buf);

    err = nvs_set_i32(nvs_handle, key, pinmap[i].pin_name);
    if (err != ESP_OK)
    {
      sprintf(debug_buf, "Failed to save pin number! Error: %d\n", err);
      uart1_debug_print(debug_buf);
      break;
    }

    // Save mode
    sprintf(key, "pin%d_mode", i);
    err = nvs_set_u8(nvs_handle, key, (uint8_t)pinmap[i].mode);
    if (err != ESP_OK)
    {
      sprintf(debug_buf, "Failed to save pin mode! Error: %d\n", err);
      uart1_debug_print(debug_buf);
      break;
    }

    // Save delay
    sprintf(key, "pin%d_delay", i);
    err = nvs_set_i32(nvs_handle, key, pinmap[i].blink_delay_ms);
    if (err != ESP_OK)
    {
      sprintf(debug_buf, "Failed to save blink delay! Error: %d\n", err);
      uart1_debug_print(debug_buf);
      break;
    }
  }

  // Commit debug
  uart1_debug_print("Committing to NVS...\n");
  err = nvs_commit(nvs_handle);
  if (err != ESP_OK)
  {
    sprintf(debug_buf, "NVS commit failed! Error: %d\n", err);
    uart1_debug_print(debug_buf);
  }

  nvs_close(nvs_handle);
  uart1_debug_print("NVS operation complete\n");
  return err;
}

esp_err_t load_pinmap_from_nvs(void)
{
  nvs_handle_t nvs_handle;
  esp_err_t err;

  // Open NVS
  err = nvs_open("gpio_config", NVS_READONLY, &nvs_handle);
  if (err != ESP_OK)
    return err;

  // Get number of used pins
  err = nvs_get_u8(nvs_handle, "used_count", &used_gpio_count);
  if (err != ESP_OK)
  {
    nvs_close(nvs_handle);
    return err;
  }

  // Load each pin configuration
  for (int i = 0; i < used_gpio_count; i++)
  {
    char key[16];

    // Load pin number
    sprintf(key, "pin%d_num", i);
    uart1_debug_print(key);
    uart1_debug_print(" Initialized.\n");
    err = nvs_get_i32(nvs_handle, key, &pinmap[i].pin_name);

    // Load mode
    sprintf(key, "pin%d_mode", i);
    uint8_t mode;
    err = nvs_get_u8(nvs_handle, key, &mode);
    pinmap[i].mode = (GPIO_MODE)mode;

    // Load blink delay
    sprintf(key, "pin%d_delay", i);
    err = nvs_get_i32(nvs_handle, key, &pinmap[i].blink_delay_ms);

    if (err != ESP_OK)
      break;

    // Initialize GPIO based on mode
    gpio_set_direction(pinmap[i].pin_name, GPIO_MODE_OUTPUT);
    pinmap[i].state = LOW;
    pinmap[i].last_wake_time = xTaskGetTickCount();
  }

  nvs_close(nvs_handle);
  return err;
}

esp_err_t save_uart_baudrates(void)
{
  nvs_handle_t nvs_handle;
  esp_err_t err;
  char debug_buf[64];

  // Initialize NVS
  err = init_nvs();
  if (err != ESP_OK)
  {
    uart1_debug_print("NVS init failed!\n");
    return err;
  }

  // Open NVS
  err = nvs_open("uart_config", NVS_READWRITE, &nvs_handle);
  if (err != ESP_OK)
  {
    sprintf(debug_buf, "NVS open failed! Error: %d\n", err);
    uart1_debug_print(debug_buf);
    return err;
  }

  // Save UART0 baudrate
  err = nvs_set_u32(nvs_handle, "uart0_baud", uart0_baudrate_);
  if (err != ESP_OK)
  {
    sprintf(debug_buf, "Failed to save UART0 baudrate! Error: %d\n", err);
    uart1_debug_print(debug_buf);
    nvs_close(nvs_handle);
    return err;
  }

  // Save UART1 baudrate
  err = nvs_set_u32(nvs_handle, "uart1_baud", uart1_baudrate_);
  if (err != ESP_OK)
  {
    sprintf(debug_buf, "Failed to save UART1 baudrate! Error: %d\n", err);
    uart1_debug_print(debug_buf);
    nvs_close(nvs_handle);
    return err;
  }

  err = nvs_commit(nvs_handle);
  nvs_close(nvs_handle);
  return err;
}

esp_err_t load_uart_baudrates(void)
{
  nvs_handle_t nvs_handle;
  esp_err_t err;
  char debug_buf[64];

  // Initialize NVS
  err = init_nvs();
  if (err != ESP_OK)
  {
    uart1_debug_print("NVS init failed!\n");
    return err;
  }

  // Open NVS
  err = nvs_open("uart_config", NVS_READONLY, &nvs_handle);
  if (err != ESP_OK)
  {
    sprintf(debug_buf, "NVS open failed! Error: %d\n", err);
    uart1_debug_print(debug_buf);
    return err;
  }

  // Load UART0 baudrate
  err = nvs_get_u32(nvs_handle, "uart0_baud", &uart0_baudrate_);
  if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
  {
    sprintf(debug_buf, "Failed to load UART0 baudrate! Error: %d\n", err);
    uart1_debug_print(debug_buf);
    nvs_close(nvs_handle);
    return err;
  }

  // Load UART1 baudrate
  err = nvs_get_u32(nvs_handle, "uart1_baud", &uart1_baudrate_);
  if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
  {
    sprintf(debug_buf, "Failed to load UART1 baudrate! Error: %d\n", err);
    uart1_debug_print(debug_buf);
    nvs_close(nvs_handle);
    return err;
  }

  nvs_close(nvs_handle);
  return ESP_OK;
}

esp_err_t save_pwm_values(void)
{
  nvs_handle_t nvs_handle;
  esp_err_t err;
  char debug_buf[64];

  // Initialize NVS
  err = init_nvs();
  if (err != ESP_OK)
  {
    uart1_debug_print("NVS init failed!\n");
    return err;
  }

  // Open NVS
  err = nvs_open("pwm_config", NVS_READWRITE, &nvs_handle);
  if (err != ESP_OK)
  {
    sprintf(debug_buf, "NVS open failed! Error: %d\n", err);
    uart1_debug_print(debug_buf);
    return err;
  }

  // Save channel 1 value
  err = nvs_set_u32(nvs_handle, "ch1_voltage", (uint32_t)(channel1_voltage_value_ * 1000));
  if (err != ESP_OK)
  {
    uart1_debug_print("Failed to save channel 1 value\n");
    nvs_close(nvs_handle);
    return err;
  }

  // Save channel 2 value
  err = nvs_set_u32(nvs_handle, "ch2_voltage", (uint32_t)(channel2_voltage_value_ * 1000));
  if (err != ESP_OK)
  {
    uart1_debug_print("Failed to save channel 2 value\n");
    nvs_close(nvs_handle);
    return err;
  }
  sprintf(debug_buf, "Saved PWM Values in NVS\n");
  uart1_debug_print(debug_buf);

  err = nvs_commit(nvs_handle);
  nvs_close(nvs_handle);
  return err;
}

esp_err_t load_pwm_values(void)
{
  nvs_handle_t nvs_handle;
  esp_err_t err;
  uint32_t value;
  char debug_buf[64];

  // Initialize NVS
  err = init_nvs();
  if (err != ESP_OK)
  {
    uart1_debug_print("NVS init failed!\n");
    return err;
  }

  // Open NVS
  err = nvs_open("pwm_config", NVS_READONLY, &nvs_handle);
  if (err != ESP_OK)
  {
    sprintf(debug_buf, "NVS open failed! Error: %d\n", err);
    uart1_debug_print(debug_buf);
    return err;
  }

  // Load channel 1 value
  err = nvs_get_u32(nvs_handle, "ch1_voltage", &value);
  if (err == ESP_OK)
  {
    channel1_voltage_value_ = value / 1000.0;
    sprintf(debug_buf, "Loaded channel1 pwm value: %.2f\n", channel1_voltage_value_);
    uart1_debug_print(debug_buf);
  }
  else if (err != ESP_ERR_NVS_NOT_FOUND)
  {
    uart1_debug_print("Failed to load channel 1 value\n");
    nvs_close(nvs_handle);
    return err;
  }

  // Load channel 2 value
  err = nvs_get_u32(nvs_handle, "ch2_voltage", &value);
  if (err == ESP_OK)
  {
    channel2_voltage_value_ = value / 1000.0;
    sprintf(debug_buf, "Loaded channel2 pwm value: %.2f\n", channel2_voltage_value_);
    uart1_debug_print(debug_buf);
  }
  else if (err != ESP_ERR_NVS_NOT_FOUND)
  {
    uart1_debug_print("Failed to load channel 2 value\n");
    nvs_close(nvs_handle);
    return err;
  }

  nvs_close(nvs_handle);
  return ESP_OK;
}