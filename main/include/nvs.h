#ifndef NVS_H
#define NVS_H
#include "nvs_flash.h"

esp_err_t init_nvs(void);

esp_err_t save_pinmap_to_nvs(void);
esp_err_t load_pinmap_from_nvs(void);

esp_err_t save_uart_baudrates(void);
esp_err_t load_uart_baudrates(void);

esp_err_t save_pwm_values(void);
esp_err_t load_pwm_values(void);

#endif // NVS_H