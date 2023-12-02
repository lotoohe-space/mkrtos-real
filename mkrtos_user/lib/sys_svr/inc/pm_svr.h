/**
 * @file pm_svr.h
 * @author ATShining (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-11-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include "u_rpc_svr.h"

typedef struct pm
{
    rpc_svr_obj_t svr_obj;
} pm_t;

void pm_svr_obj_init(pm_t *pm);
int pm_rpc_run_app(const char *path);
