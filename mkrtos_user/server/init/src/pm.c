/**
 * @file pm.c
 * @author ATShining (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-11-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "pm_svr.h"
#include "u_app_loader.h"
#include "u_env.h"
#include "rpc_prot.h"
#include <stdio.h>

static pm_t pm;

void pm_init(void)
{
    pm_svr_obj_init(&pm);
    meta_reg_svr_obj(&pm.svr_obj, PM_PROT);
    printf("pm runing..\n");
}

int pm_rpc_run_app(const char *path)
{
    printf("pm run %s.\n", path);
    return app_load(path, u_get_global_env());
}
