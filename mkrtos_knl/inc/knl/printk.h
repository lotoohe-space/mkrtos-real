/**
 * @file printk.h
 * @author zhagnzheng (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-09-14
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include "kobject.h"
#include "types.h"
#include "arch.h"
/**
 * @brief 向io输出一个字符
 *
 * @param c
 */
void putc(int c);
/**
 * @brief 获取io的一个输入
 *
 * @return int
 */
int getc(void);
/**
 * @brief 打印内核信息
 *
 * @param fmt
 * @param ...
 */
void printk(const char *fmt, ...);

void dumpstack(void);

/**
 * @brief 可变参数宏
 *
 */
#define dbg_print_kobj(kobj, fmt, ...) printk(kobject_get_name(kobj), "[%s]" fmt, __VA_ARGS__)
#define dbg_printk(fmt, ...) printk("[%d]" fmt, sys_tick_cnt_get(), __VA_ARGS__)