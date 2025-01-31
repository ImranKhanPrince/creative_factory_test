#include "nvs.h"
#include "uart1.h"
#include "uart0.h"
#include "gpio.h"
#include "AO.h"

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

esp_err_t save_pinmap_to_nvs(void)
{
  nvs_handle_t nvs_handle;
  esp_err_t err;

  err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
  if (err != ESP_OK)
  {
    uart1_log("NVS open failed: %s\n", esp_err_to_name(err));
    return err;
  }

  err = nvs_set_blob(nvs_handle, "pinmap", pinmap, sizeof(pinMap_t) * MAX_PINS);
  if (err != ESP_OK)
  {
    uart1_log("NVS write failed: %s\n", esp_err_to_name(err));
    nvs_close(nvs_handle);
    return err;
  }

  err = nvs_commit(nvs_handle);
  if (err != ESP_OK)
  {
    uart1_log("NVS commit failed: %s\n", esp_err_to_name(err));
  }

  nvs_close(nvs_handle);
  uart1_log("Pinmap saved to NVS\n");
  return err;
}

esp_err_t load_pinmap_from_nvs(void)
{
  nvs_handle_t nvs_handle;
  esp_err_t err;
  size_t required_size = sizeof(pinMap_t) * MAX_PINS;

  err = nvs_open("storage", NVS_READONLY, &nvs_handle);
  if (err != ESP_OK)
  {
    uart1_log("NVS open failed: %s\n", esp_err_to_name(err));
    return err;
  }

  err = nvs_get_blob(nvs_handle, "pinmap", pinmap, &required_size);
  if (err != ESP_OK)
  {
    uart1_log("NVS read failed: %s\n", esp_err_to_name(err));
  }

  nvs_close(nvs_handle);
  uart1_log("Pinmap loaded from NVS\n");
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