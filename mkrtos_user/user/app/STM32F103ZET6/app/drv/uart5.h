#pragma once

#include "stm32_sys.h"
void init_uart5(u32 baudRate);
void uart5_send_string(char *string);
void uart5_send_bytes(u8 *bytes, int len);
void uart5_send_byte(u8 byte);
