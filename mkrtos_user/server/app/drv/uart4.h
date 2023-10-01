#pragma once

#include "stm32_sys.h"
void init_uart4(u32 baudRate);
void uart4_send_string(char *string);
void uart4_send_bytes(u8 *bytes, int len);
void uart4_send_byte(u8 byte);
