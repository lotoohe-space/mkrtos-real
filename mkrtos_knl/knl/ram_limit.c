/**
 * @file ram_limit.c
 * @author ATShining (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-29
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "ram_limit.h"
#include "types.h"
void ram_limit_init(ram_limit_t *limit, size_t max)
{
    limit->max = max;
    limit->cur = 0;
}
bool_t ram_limit_alloc(ram_limit_t *limit, size_t size)
{
    if (limit->max == 0)
    {
        return TRUE;
    }
    if (size + limit->cur > limit->max)
    {
        return FALSE;
    }
    limit->cur += size;
    return TRUE;
}
void ram_limit_free(ram_limit_t *limit, size_t size)
{
    if (limit->max == 0)
    {
        return;
    }
    limit->cur -= size;
}