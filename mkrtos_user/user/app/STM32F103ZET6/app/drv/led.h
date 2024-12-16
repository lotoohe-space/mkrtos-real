#pragma once

#include "stm32_sys.h"
void led_init(void);

#define toogle_led_0() PCout(13) = !PCin(13)