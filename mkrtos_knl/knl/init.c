/**
 * @file init.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-09-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "types.h"
#include "init.h"

extern char *__init_array_start[]; //!< 自动调用函数的开始
extern char *__init_array_end[];   //!< 自动调用函数的结尾

/**
 * @brief 自动调用初始化函数
 *
 */
void sys_call_init(void)
{
    uintptr_t a = (uintptr_t)&__init_array_start;

    for (; a < (uintptr_t)&__init_array_end; a += sizeof(void (*)()))
        ((init_func)(*(uintptr_t *)a))();
}
