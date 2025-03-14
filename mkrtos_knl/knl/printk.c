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
#include "thread.h"
#include "pre_cpu.h"
static spinlock_t lock;
static char print_cache[CONFIG_PRINTK_CACHE_SIZE];

/**
 * @brief 原始输出
 *
 * @param str
 */
static void print_raw(const char *str)
{
    for (int i = 0; str[i]; i++)
    {
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
    thread_t *cut_th = thread_get_current();

    state = spinlock_lock(&lock);
    xsprintf(print_cache, "[%8d]%s: ",
             pre_cpu_is_init() ? sys_tick_cnt_get() : 0,
             kobject_get_name(&cut_th->kobj));
    print_raw(print_cache);
    va_start(args, fmt);
    xvsprintf(print_cache, fmt, args);
    va_end(args);
    print_raw(print_cache);
    spinlock_set(&lock, state);
}
