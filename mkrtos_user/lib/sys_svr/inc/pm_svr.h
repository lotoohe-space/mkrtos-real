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

#include "pm_cli.h"
#include "u_rpc_svr.h"
#include "u_slist.h"
#include "u_types.h"
typedef struct watch_entry
{
    pid_t watch_pid;//!<被监控的pid
    pid_t src_pid; //!<发起监控的pid
    obj_handler_t sig_hd; //!<用于通信用的ipc对象
    #if 0
    obj_handler_t notify_sem_hd;//!<通知用的信号量
    #endif
    int flags; //!<暂时没有用到
    slist_head_t node;//!<双向链表串联起来
} watch_entry_t;

typedef struct pm
{
    rpc_svr_obj_t svr_obj;
    slist_head_t watch_head;
} pm_t;

#define PM_APP_BG_RUN 0x1

void pm_svr_obj_init(pm_t *pm);
int pm_rpc_run_app(const char *path, int mem_block, char *params, int params_len, char *env, int envs_len);
int pm_rpc_kill_task(int src_pid, int pid, int flags, int exit_code);

int pm_rpc_watch_pid(pm_t *pm, obj_handler_t sig_rcv_hd, pid_t pid, int flags);
int pm_rpc_copy_data(pid_t src_pid, pid_t dst_pid, umword_t src_addr, umword_t dst_addr, size_t len);
int pm_rpc_del_watch_pid(pm_t *pm, pid_t pid, int flags);
