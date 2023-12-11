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
#include "u_task.h"
#include <errno.h>
static pm_t pm;

void pm_init(void)
{
    pm_svr_obj_init(&pm);
    meta_reg_svr_obj(&pm.svr_obj, PM_PROT);
    printf("pm runing..\n");
}
int pm_rpc_kill_task(int pid, int flags)
{
    int obj_type;
    msg_tag_t tag;

    if (pid == TASK_THIS)
    {
        return -EINVAL;
    }

    tag = task_obj_valid(TASK_THIS, pid, &obj_type);
    if (msg_tag_get_val(tag) < 0)
    {
        return msg_tag_get_val(tag);
    }
    if (msg_tag_get_val(tag) == 0)
    {
        return -1;
    }
    if (obj_type != TASK_TYPE)
    {
        return -1;
    }
    printf("[pm] kill pid:%d.\n", pid);
    ns_node_del_by_pid(pid, flags);
    task_unmap(TASK_THIS, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, pid));
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
