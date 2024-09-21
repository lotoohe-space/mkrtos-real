/**
 * @file util.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-09-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "types.h"

/**
 * @brief 获取x的最低有效位
 *
 * @param x 需要获取最低有效位的数
 * @return int 最低有效位的位置
 */
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
/**
 * @brief 获取x的前导零的个数
 *
 * @param x 需要获取前导零的个数的数
 * @return int 前导零的个数
 */
int clz(umword_t x)
{
    int ret;

    __asm__ volatile("clz\t%0, %1"
                     : "=r"(ret)
                     : "r"(x)
                     : "cc");
    return ret;
}