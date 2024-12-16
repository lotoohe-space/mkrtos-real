#pragma once

#include "stm32_sys.h"

extern __IO uint16_t ADCConvertedValue[6];

// 得到输出电压 mV
#define ADC_OUT_V(a) (((a) / 4096.0f * 3300) * 2)

void adc_init(void);

