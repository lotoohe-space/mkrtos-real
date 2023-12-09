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
#include "cons_svr.h"
#include "namespace.h"
static pm_t pm;

void pm_init(void)
{
    pm_svr_obj_init(&pm);
    meta_reg_svr_obj(&pm.svr_obj, PM_PROT);
    printf("pm runing..\n");
}

int pm_rpc_kill_task(int pid, int flags)
{
    printf("[pm] kill pid:%d.\n", pid);
    ns_node_del_by_pid(pid);
    return 0;
}
int pm_rpc_run_app(const char *path, int flags)
{
    pid_t pid;
    int ret;
    printf("pm run %s.\n", path);

    ret = app_load(path, u_get_global_env(), &pid, NULL, 0);
    if (ret > 0)
    {
        if (!(flags & PM_APP_BG_RUN))
        {
            console_active(pid);
        }
    }
}
