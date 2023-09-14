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

#include "types.h"

void putc(int c);
int getc(void);
void printk(const char *fmt, ...);
