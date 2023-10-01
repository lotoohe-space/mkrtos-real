#pragma once

#include "stm32_sys.h"
void init_usart3(u32 baudRate);
void usart3_send_string(char *string);
void usart3_send_bytes(u8 *bytes, int len);
void usart3_send_byte(u8 byte);
