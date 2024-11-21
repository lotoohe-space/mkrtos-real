/**
 * @file pm.h
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-11-28
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include <u_types.h>

void pm_init(void);

static inline obj_handler_t pm_hd2pid(pid_t _pid)
{
    return (obj_handler_t)_pid;
}
