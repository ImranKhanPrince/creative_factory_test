#ifndef NVS_H
#define NVS_H
#include "nvs_flash.h"

#include "gpio.h"

// TODO: save and load for each entry
// uart baud rate
// gpio matrix. with pin value or blink state
// analog out value
esp_err_t init_nvs(void);

esp_err_t save_pinmap_to_nvs(void);
esp_err_t load_pinmap_from_nvs(void);

esp_err_t save_uart_baudrates(void);
esp_err_t load_uart_baudrates(void);

#endif // NVS_H