/**
 * @file printk.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-09-14
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "printk.h"
#include "spinlock.h"
#include "stdarg.h"
#include "uart/uart.h"
#include "xprintf.h"

static spinlock_t lock;
static char print_cache[CONFIG_PRINTK_CACHE_SIZE];

/**
 * @brief 原始输出
 *
 * @param str
 */
static void print_raw(const char *str)
{
    for (int i = 0; str[i]; i++) {
        uart_putc(uart_get_global(), str[i]);
    }
}
/**
 * @brief 发送一个数据
 *
 * @param c
 */
void putc(int c)
{
    uart_putc(uart_get_global(), c);
}
int getc(void)
{
    return uart_getc(uart_get_global());
}
/**
 * @brief 输出数据
 *
 * @param fmt
 * @param ...
 */
void printk(const char *fmt, ...)
{
    va_list args;
    umword_t state = 0;

    state = spinlock_lock(&lock);
    va_start(args, fmt);
    xvsprintf(print_cache, fmt, args);
    va_end(args);
    print_raw(print_cache);
    spinlock_set(&lock, state);
}

void dumpstack(void)
{
    uint64_t result[5];
    umword_t x29;
    umword_t status = cpulock_lock();

    x29 = read_nmreg(x29);
    for (size_t i = 0; i < sizeof(result) / sizeof(umword_t); i++) {
        result[i] = (*(umword_t *)(x29 + 8)) - 4;
        x29 = *(umword_t *)(x29);
    }
    printk("[knl dump stack]: 0x%lx,0x%lx,0x%lx,0x%lx,0x%lx\n", result[0], result[1], result[2], result[3], result[4]);
    cpulock_set(status);
}
