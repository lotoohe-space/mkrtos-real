/**
 * @file u_sig.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-08-27
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <u_thread_util.h>
#include <u_util.h>
#include <u_rpc_svr.h>
#include <u_factory.h>
#include <u_sleep.h>
#include <u_sig.h>
#include <pm_cli.h>
#include <rpc_prot.h>
#include <assert.h>
#include <errno.h>
#include "u_fast_ipc.h"
#include "u_hd_man.h"
#include "u_rpc_svr.h"
#include "sig_svr.h"
#ifdef CONFIG_USING_SIG

static sig_t sig_obj;

static sig_call_back sig_cb_func;
static obj_handler_t sig_ipc = HANDLER_INVALID;
static uint8_t sig_init_flags;

int pm_sig_watch(pid_t pid, int flags)
{
    int ret = pm_watch_pid(sig_ipc, pid, flags);

    return ret;
}
int pm_sig_del_watch(pid_t pid, int flags)
{
    return pm_del_watch_pid(pid, flags);
}
void pm_sig_func_set(sig_call_back sig_func)
{
    sig_cb_func = sig_func;
}

static int kill(int flags, int pid)
{
    int ret = -EINVAL;

    if (sig_cb_func)
    {
        ret = sig_cb_func(pid, flags);
    }
    return ret;
}
static const sig_op_t sig_op = {
    .kill = kill,
    .to_kill = NULL,
};
void sig_init(void)
{
    if (sig_init_flags)
    {
        return;
    }
    rpc_meta_init(TASK_THIS, &sig_ipc);
    // 注册到队列中去
    sig_svr_obj_init(&sig_obj);
    sig_obj.op = &sig_op;
    meta_obj_init();
    meta_reg_svr_obj(&sig_obj.svr_obj, SIG_PORT);
}
#endif