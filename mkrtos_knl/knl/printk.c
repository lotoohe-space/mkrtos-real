/*
 * @Author: zhangzheng 1358745329@qq.com
 * @Date: 2023-08-14 09:47:54
 * @LastEditors: zhangzheng 1358745329@qq.com
 * @LastEditTime: 2023-08-14 13:11:18
 * @FilePath: /mkrtos-real/mkrtos_knl/knl/printk.c
 * @Description: 内核信息输出，内核内使用
 */
#include "printk.h"
#include "stdarg.h"
#include "xprintf.h"
#include "spinlock.h"

static spinlock_t lock;
static char print_cache[128];

/**
 * @brief 原始输出
 *
 * @param str
 */
static void print_raw(const char *str)
{
    for (int i = 0; str[i]; i++)
    {
        xputc(str[i]);
    }
}
/**
 * @brief 发送一个数据
 *
 * @param c
 */
void putc(int c)
{
    xputc(c);
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

    spinlock_lock(&lock);
    va_start(args, fmt);
    xvsprintf(print_cache, fmt, args);
    va_end(args);
    print_raw(print_cache);
    spinlock_set(&lock, state);
}