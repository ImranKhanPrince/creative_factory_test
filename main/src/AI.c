#include "AI.h"
#include "uart1.h"

static adc_oneshot_unit_handle_t adc1_handle;

// STATIC SIGNATUES
static void init_adc();

static void init_adc()
{
  if (adc1_handle == NULL)
  {
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_new_unit(&init_config, &adc1_handle);
  }
}

void init_adc_channels()
{
  init_adc();
  adc_oneshot_chan_cfg_t config = {
      .bitwidth = ADC_BITWIDTH,
      .atten = ADC_ATTEN,
  };
  adc_oneshot_config_channel(adc1_handle, CHANNEL1, &config);
  adc_oneshot_config_channel(adc1_handle, CHANNEL2, &config);
}

int read_adc_channel1()
{
  int adc_raw;
  adc_oneshot_read(adc1_handle, CHANNEL1, &adc_raw);
  uart1_log("GPIO34 ADC Raw: %d\n", adc_raw);
  return adc_raw;
}

int read_adc_channel2()
{
  int adc_raw;
  adc_oneshot_read(adc1_handle, CHANNEL2, &adc_raw);
  uart1_log("GPIO35 ADC Raw: %d\n", adc_raw);
  return adc_raw;
}

void cleanup_adc()
{
  if (adc1_handle)
  {
    adc_oneshot_del_unit(adc1_handle);
  }
}