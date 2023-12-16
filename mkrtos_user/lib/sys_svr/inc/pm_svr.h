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
#include "u_slist.h"
#include "u_types.h"
typedef struct watch_entry
{
    pid_t watch_pid;
    pid_t src_pid;
    obj_handler_t sig_hd;
    int flags;
    slist_head_t node;
} watch_entry_t;

typedef struct pm
{
    rpc_svr_obj_t svr_obj;
    slist_head_t watch_head;
} pm_t;

#define PM_APP_BG_RUN 0x1

void pm_svr_obj_init(pm_t *pm);
int pm_rpc_run_app(const char *path, int flags);
int pm_rpc_kill_task(int pid, int flags);
int pm_rpc_watch_pid(pm_t *pm, obj_handler_t sig_rcv_hd, pid_t pid, int flags);
