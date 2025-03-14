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
#include "u_sema.h"
#include "u_task.h"
#ifdef CONFIG_USING_SIG

static sig_t sig_obj;

static sig_call_back sig_cb_func;
static obj_handler_t sig_ipc = HANDLER_INVALID;
static uint8_t sig_init_flags;
static pid_t wait_pid;
static obj_handler_t sema_wait_hd;

int pm_sig_watch(pid_t pid, int flags)
{
    int ret = pm_watch_pid(sig_ipc, pid, flags);

    return ret;
}
int pm_sig_del_watch(pid_t pid, int flags)
{
    return pm_del_watch_pid(pid, flags);
}
/**
 * FIXME:对于已经挂了的进程，暂时还不支持waitpid。
 */
int pm_waitpid(pid_t pid, umword_t *status)
{
    msg_tag_t tag;

    wait_pid = pid;
    pm_sig_watch(pid, 0);
    u_sema_down(sema_wait_hd, 0, NULL);
    if (status) {
        *status = 0;/*FIXME:*/
    }
    return 0;
}
sig_call_back pm_sig_func_set(sig_call_back sig_func)
{
    sig_call_back tmp = sig_cb_func;

    sig_cb_func = sig_func;
    return tmp;
}

static int kill(int flags, int pid)
{
    int ret = -EINVAL;

    if (wait_pid == pid || wait_pid == -1)
    {
        u_sema_up(sema_wait_hd);
    }
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
    msg_tag_t tag;

    if (sig_init_flags)
    {
        return;
    }
    rpc_meta_init_def(TASK_THIS, &sig_ipc);
    // 注册到队列中去
    sig_svr_obj_init(&sig_obj);
    sig_obj.op = &sig_op;
    meta_obj_init_def();
    meta_reg_svr_obj(&sig_obj.svr_obj, SIG_PORT);
    sema_wait_hd = handler_alloc();
    assert(sema_wait_hd != HANDLER_INVALID);
    tag = facotry_create_sema(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, sema_wait_hd), 0, 1);
    assert(msg_tag_get_val(tag) >= 0);
}
#endif