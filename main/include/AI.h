#ifndef AI_H
#define AI_H

#include <stdint.h>
#include "driver/adc.h"

// ADC Channels definitions
#define MAX_AI_CHANNELS 8
#define ADC_ATTEN ADC_ATTEN_DB_11

#define ADC1_CHANNEL_4 ADC_CHANNEL_4 // GPIO32
#define ADC1_CHANNEL_5 ADC_CHANNEL_5 // GPIO33
#define ADC_BITWIDTH ADC_BITWIDTH_10 // making adc into 10bit insted of 12bit(default)

// Function prototypes
void init_adc_channel1();
void init_adc_channel2();

int read_adc_channel1();
int read_adc_channel2();
void cleanup_adc();

#endif // AI_H