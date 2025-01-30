#ifndef AI_H
#define AI_H

#include <stdint.h>
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

// ADC Configuration

#define ADC_ATTEN ADC_ATTEN_DB_12
#define ADC_BITWIDTH ADC_BITWIDTH_10
#define MAX_AI_CHANNELS 2

// Define ADC channels using ESP-IDF 5.3 naming convention
#define CHANNEL1 ADC_CHANNEL_6 // GPIO34
#define CHANNEL2 ADC_CHANNEL_7 // GPIO35

#define ADC_VREF 3.3f
#define ADC_MAX ((1 << ADC_BITWIDTH) - 1)

// Function prototypes
void init_adc_channels();

int read_adc_channel1();
int read_adc_channel2();
void cleanup_adc();

#endif // AI_H
