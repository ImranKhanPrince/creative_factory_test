#include "AI.h"
#include "uart1.h"

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

static adc_oneshot_unit_handle_t adc1_handle;
static adc_cali_handle_t adc1_cali_handle_ch1, adc1_cali_handle_ch2;
static bool do_calibration_ch1 = false, do_calibration_ch2 = false;

// Function to initialize ADC unit (if not already initialized)
void init_adc_unit()
{
  if (adc1_handle == NULL)
  {
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_new_unit(&init_config, &adc1_handle);
  }
}

// Function to initialize ADC for GPIO32 (Channel 1)
void init_adc_channel1()
{
  init_adc_unit();

  adc_oneshot_chan_cfg_t config = {
      .bitwidth = ADC_BITWIDTH,
      .atten = ADC_ATTEN // 0-3.6V range
  };
  adc_oneshot_config_channel(adc1_handle, ADC1_CHANNEL_4, &config);

  // Calibration setup for channel 1
  adc_cali_line_fitting_config_t cali_config = {
      .unit_id = ADC_UNIT_1,
      .atten = ADC_ATTEN,
      .bitwidth = ADC_BITWIDTH};
  if (adc_cali_create_scheme_line_fitting(&cali_config, &adc1_cali_handle_ch1) == ESP_OK)
  {
    do_calibration_ch1 = true;
  }
}

// Function to initialize ADC for GPIO33 (Channel 2)
void init_adc_channel2()
{
  init_adc_unit();

  adc_oneshot_chan_cfg_t config = {
      .bitwidth = ADC_BITWIDTH,
      .atten = ADC_ATTEN // 0-3.6V range
  };
  adc_oneshot_config_channel(adc1_handle, ADC1_CHANNEL_5, &config);

  // Calibration setup for channel 2
  adc_cali_line_fitting_config_t cali_config = {
      .unit_id = ADC_UNIT_1,
      .atten = ADC_ATTEN,
      .bitwidth = ADC_BITWIDTH};
  if (adc_cali_create_scheme_line_fitting(&cali_config, &adc1_cali_handle_ch2) == ESP_OK)
  {
    do_calibration_ch2 = true;
  }
}

// Function to read ADC value from GPIO32 (Channel 1)
int read_adc_channel1()
{
  int adc_raw;
  adc_oneshot_read(adc1_handle, ADC1_CHANNEL_4, &adc_raw);

  if (do_calibration_ch1)
  {
    int voltage;
    adc_cali_raw_to_voltage(adc1_cali_handle_ch1, adc_raw, &voltage);
    uart1_log("GPIO32 ADC Raw: %d, Voltage: %d mV\n", adc_raw, voltage); // TODO: change with
    return voltage;
  }

  uart1_log("GPIO32 ADC Raw: %d\n", adc_raw);
  return adc_raw;
}

// Function to read ADC value from GPIO33 (Channel 2)
int read_adc_channel2()
{
  int adc_raw;
  adc_oneshot_read(adc1_handle, ADC1_CHANNEL_5, &adc_raw);

  if (do_calibration_ch2)
  {
    int voltage;
    adc_cali_raw_to_voltage(adc1_cali_handle_ch2, adc_raw, &voltage);
    uart1_log("GPIO33 ADC Raw: %d, Voltage: %d mV\n", adc_raw, voltage);
    return voltage;
  }

  uart1_log("GPIO33 ADC Raw: %d\n", adc_raw);
  return adc_raw;
}

// Cleanup function
void cleanup_adc()
{
  if (adc1_handle)
  {
    adc_oneshot_del_unit(adc1_handle);
  }
  if (do_calibration_ch1)
  {
    adc_cali_delete_scheme_line_fitting(adc1_cali_handle_ch1);
  }
  if (do_calibration_ch2)
  {
    adc_cali_delete_scheme_line_fitting(adc1_cali_handle_ch2);
  }
}
