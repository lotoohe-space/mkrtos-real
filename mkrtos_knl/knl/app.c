/**
 * @file app.c
 * @author ATShining (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-29
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "types.h"
#include "app.h"

/**
 * @brief 通过地址获得app的信息
 * 
 * @param addr 内存地址
 * @return app_info_t* app的信息
 */
app_info_t *app_info_get(void *addr)
{
    return (app_info_t *)addr;
}
