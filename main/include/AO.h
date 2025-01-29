#ifndef AO_H
#define AO_H

#include <stdbool.h>
#include "driver/ledc.h"

extern bool channel1_running_;
extern bool channel2_running_;

extern double channel1_voltage_value_;
extern double channel2_voltage_value_;

esp_err_t start_pwm_channel1();
esp_err_t start_pwm_channel2();

void set_pwm_voltage(int channel_num, float voltage);

#endif // UART2_HEADER