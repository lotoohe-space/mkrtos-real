
#pragma once

#include "types.h"

typedef struct uart
{
    int baud;
    uint8_t stop;
    uint8_t parity;
    uint8_t word_len;
    uint8_t hardware_flow_rts;
} uart_t;

uart_t *uart_get_global(void);
void uart_set(uart_t *uart);
void uart_putc(uart_t *uart, int data);
int uart_getc(uart_t *uart);