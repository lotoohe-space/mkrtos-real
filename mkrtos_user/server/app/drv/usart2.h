#pragma once
#include "stm32_sys.h"

#define USART2_RECV_CACHE_MAX_LEN 256

typedef void (*usart_recv_data_fun)(u8 *data, u16 len);

void init_usart2(u32 BaudRate);
void usart2_set_recv_data_cb(usart_recv_data_fun usart_recv_fun);

void usart2_over_time_check(void);

void usart2_send_byte(u8 byte);
void usart2_send_bytes(u8 *bytes, int len);
void usart2_send_string(char *string);

void usart2_loop(void);
