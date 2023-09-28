/**
 * @file util.c
 * @author zhangzheng (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-29
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "types.h"

int ffs(int x)
{
    int ret;

    __asm__ volatile("clz\t%0, %1"
                     : "=r"(ret)
                     : "r"(x)
                     : "cc");
    ret = (WORD_BITS - 1) - ret;
    return ret;
}
