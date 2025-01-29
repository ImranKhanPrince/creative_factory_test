#include "AO.h"
#include "uart1.h"

// EPS32 has a 8bit DAC and 10bit PWM so using PWM
#define LEDC0_TIMER LEDC_TIMER_0
#define LEDC1_TIMER LEDC_TIMER_1

#define LEDC_MODE LEDC_HIGH_SPEED_MODE
#define LEDC_FREQUENCY 5000               // 5 kHz
#define LEDC_RESOLUTION LEDC_TIMER_10_BIT // 10-bit resolution (0-1023)
#define MAX_VOLTAGE 3.3                   // Max ESP32 voltage

#define CHANNEL1_PIN_NO GPIO_NUM_22
#define CHANNEL2_PIN_NO GPIO_NUM_23

bool channel1_running_ = false;
bool channel2_running_ = false;

double channel1_voltage_value_ = 0;
double channel2_voltage_value_ = 0;

esp_err_t start_pwm_channel1()
{
  esp_err_t err;
  ledc_timer_config_t ledc_timer = {
      .speed_mode = LEDC_MODE,
      .timer_num = LEDC0_TIMER, // Timer Num
      .duty_resolution = LEDC_RESOLUTION,
      .freq_hz = LEDC_FREQUENCY,
      .clk_cfg = LEDC_AUTO_CLK};
  ledc_timer_config(&ledc_timer);
  err = ledc_timer_config(&ledc_timer);
  if (err != ESP_OK)
    return err;

  ledc_channel_config_t ledc_channel = {
      .gpio_num = CHANNEL1_PIN_NO,
      .speed_mode = LEDC_MODE,
      .channel = LEDC_CHANNEL_0,
      .timer_sel = LEDC0_TIMER, // Timer selection
      .duty = (int)((channel1_voltage_value_ / MAX_VOLTAGE) * ((1 << LEDC_RESOLUTION) - 1)),
      .hpoint = 0};
  err = ledc_channel_config(&ledc_channel);
  if (err == ESP_OK)
    channel1_running_ = true;
  return err;
}

esp_err_t start_pwm_channel2()
{
  esp_err_t err;
  ledc_timer_config_t ledc_timer = {
      .speed_mode = LEDC_MODE,
      .timer_num = LEDC1_TIMER, // Timer num
      .duty_resolution = LEDC_RESOLUTION,
      .freq_hz = LEDC_FREQUENCY,
      .clk_cfg = LEDC_AUTO_CLK};
  err = ledc_timer_config(&ledc_timer);
  if (err != ESP_OK)
    return err;

  ledc_channel_config_t ledc_channel = {
      .gpio_num = CHANNEL2_PIN_NO,
      .speed_mode = LEDC_MODE,
      .channel = LEDC_CHANNEL_1,
      .timer_sel = LEDC1_TIMER, // Timer selection
      .duty = (int)((channel2_voltage_value_ / MAX_VOLTAGE) * ((1 << LEDC_RESOLUTION) - 1)),
      .hpoint = 0};
  err = ledc_channel_config(&ledc_channel);
  if (err == ESP_OK)
    channel1_running_ = true;
  return err;
}

void set_pwm_voltage(int channel_num, float voltage)
{
  if (channel_num == 1)
  {
    if (voltage < 0)
      voltage = 0;
    if (voltage > MAX_VOLTAGE)
      voltage = MAX_VOLTAGE;
    int duty = (int)((voltage / MAX_VOLTAGE) * ((1 << LEDC_RESOLUTION) - 1)); // (2*10)-1

    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_0);
    channel1_voltage_value_ = voltage;
    uart1_log("Voltage set to %.2f\n", voltage);
  }
  else if (channel_num == 2)
  {
    if (voltage < 0)
      voltage = 0;
    if (voltage > MAX_VOLTAGE)
      voltage = MAX_VOLTAGE;
    int duty = (int)((voltage / MAX_VOLTAGE) * ((1 << LEDC_RESOLUTION) - 1)); // (2*10)-1

    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, duty);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1);
    channel2_voltage_value_ = voltage;
    uart1_log("Voltage set to %.2f\n", voltage);
  }
  else
  {
    uart1_debug_print("Wrong PWM channel\n");
  }

  // printf("Set pin %d to %.2fV (Duty: %d/%d)\n", pinNumber, voltage, duty, (1 << LEDC_RESOLUTION) - 1);
}
