/**
 * @file cpulock.c
 * @author ATShining (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-29
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "types.h"
#include "arch.h"
/**
 * @brief 锁住当前cpu
 * 
 * @return umword_t 
 */
umword_t cpulock_lock(void)
{
    umword_t res;

    res = intr_status();
    sti();
    return res;
}
/**
 * @brief 获取cpu锁的状态
 * 
 * @return umword_t 
 */
umword_t cpulock_get_status(void)
{
    umword_t res;

    res = intr_status();
    return res;
}
/**
 * @brief 设置cpu锁的状态
 * 
 * @param s 
 */
void cpulock_set(umword_t s)
{
    if (s)
    {
        sti();
    }
    else
    {
        cli();
    }
}
