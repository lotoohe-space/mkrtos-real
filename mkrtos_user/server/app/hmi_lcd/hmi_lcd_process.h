#pragma once

#include "hmi_driver.h"
#include "hmi_user_uart.h"
#include "cmd_queue.h"
#include "cmd_process.h"

void SilderSetValue(u8 index, u16 value);
void RelaySetButtonValue(u8 index, u8 value);
void ProcessMessage(PCTRL_MSG msg, uint16_t size);
void UpdateUI(void);

void LCD_init(void);
