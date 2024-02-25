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
#include "cons_svr.h"
#include "namespace.h"
#include "u_task.h"
#include "u_hd_man.h"
#include "u_sig.h"
#include <errno.h>
#include <malloc.h>
#include <stdio.h>
#include <assert.h>
static pm_t pm;

void pm_init(void)
{
    pm_svr_obj_init(&pm);
    meta_reg_svr_obj(&pm.svr_obj, PM_PROT);
    // printf("pm runing..\n");
}
/**
 * @brief pid值是不是一个task
 *
 * @param pid
 * @return bool_t
 */
bool_t pm_pid_is_task(pid_t pid)
{
    int obj_type;
    msg_tag_t tag = task_obj_valid(TASK_THIS, pid, &obj_type);
    if (msg_tag_get_val(tag) < 0)
    {
        return FALSE;
    }
    if (msg_tag_get_val(tag) == 0)
    {
        return FALSE;
    }
    if (obj_type != TASK_TYPE)
    {
        return FALSE;
    }
    return TRUE;
}
/**
 * @brief 查找某个观察项
 *
 * @param pm
 * @param pid
 * @return watch_entry_t*
 */
watch_entry_t *pm_watch_lookup(pm_t *pm, pid_t pid)
{
    watch_entry_t *pos;

    slist_foreach_not_next(pos, &pm->watch_head, node)
    {
        watch_entry_t *next = slist_next_entry(pos, &pm->watch_head, node);
        if (pos->src_pid == pid)
        {
            return pos;
        }
        pos = next;
    }
    return NULL;
}
/**
 * @brief 删除某个监听者
 *
 * @param pm
 * @param pid 要删除的pid
 */
void pm_del_watch_by_pid(pm_t *pm, pid_t pid)
{
    watch_entry_t *pos;

    slist_foreach_not_next(pos, &pm->watch_head, node)
    {
        watch_entry_t *next = slist_next_entry(pos, &pm->watch_head, node);
        if (pos->src_pid == pid)
        {
            slist_del(&pos->node);
            handler_free_umap(pos->sig_hd);
            free(pos);
        }
        pos = next;
    }
}
/**
 * @brief 源pid监听某个pid的状态
 *
 * @param pid 被监听的状态
 * @param flags 监听的flags
 * @return int >=0 success <0 fail
 */
int pm_rpc_watch_pid(pm_t *pm, obj_handler_t sig_rcv_hd, pid_t pid, int flags)
{
    pid_t src_pid = thread_get_src_pid();

    if (pm_pid_is_task(src_pid) == FALSE)
    {
        return -EINVAL;
    }
    if (pm_watch_lookup(pm, src_pid))
    {
        handler_free_umap(sig_rcv_hd);
        return -EEXIST;
    }
    watch_entry_t *entry = (watch_entry_t *)malloc(sizeof(watch_entry_t));

    if (!entry)
    {
        handler_free_umap(sig_rcv_hd);
        return -ENOMEM;
    }

    entry->sig_hd = sig_rcv_hd;
    entry->src_pid = src_pid;
    entry->watch_pid = pid;
    entry->flags = flags;
    slist_init(&entry->node);
    slist_add_append(&pm->watch_head, &entry->node);
    printf("[pm] watch pid:%d, sig hd:%d.\n", src_pid, sig_rcv_hd);
    return 0;
}
/**
 * @brief pm给task的信号线程发送消息
 *
 * @param pm
 * @param pid
 * @return bool_t
 */
static bool_t pm_send_sig_to_task(pm_t *pm, pid_t pid, umword_t sig_val)
{
    ipc_msg_t *ipc;
    watch_entry_t *pos;

    ipc = thread_get_cur_ipc_msg();
    assert(ipc);

    slist_foreach_not_next(pos, &pm->watch_head, node)
    {
        watch_entry_t *next = slist_next_entry(pos, &pm->watch_head, node);

        if (pos->watch_pid == pid)
        {
            if (sig_val == KILL_SIG)
            {
                ipc->msg_buf[0] = PM_SIG_NOTIFY;
                ipc->msg_buf[1] = sig_val;
                ipc->msg_buf[2] = pid;

                thread_ipc_call(msg_tag_init4(0, 3, 0, PM_SIG_PROT), pos->sig_hd,
                                ipc_timeout_create2(0, 0));
            }
            slist_del(&pos->node);
            handler_free_umap(pos->sig_hd);   //!< 删除信号通知的ipc
            handler_del_umap(pos->watch_pid); //!< 删除被watch的进程
            free(pos);
        }
        pos = next;
    }
}
/**
 * @brief 杀死某个进程
 *
 * @param pid
 * @param flags
 * @return int
 */
int pm_rpc_kill_task(int pid, int flags)
{
    if (pid == TASK_THIS)
    {
        return -EINVAL;
    }
    if (pm_pid_is_task(pid) == FALSE)
    {
        return -EINVAL;
    }

    ns_node_del_by_pid(pid, flags);          //!< 从ns中删除
    pm_del_watch_by_pid(&pm, pid);           //!< 从watch中删除
    pm_send_sig_to_task(&pm, pid, KILL_SIG); //!< 给watch者发送sig
    handler_del_umap(pid);
    printf("[pm] kill pid:%d.\n", pid);
    return 0;
}
/**
 * @brief 运行一个新的app
 *
 * @param path
 * @param flags
 * @return int
 */
int pm_rpc_run_app(const char *path, int flags)
{
    pid_t pid;
    int ret;
    printf("pm run %s.\n", path);

    ret = app_load(path, u_get_global_env(), &pid, NULL, 0, NULL, 0);
    if (ret > 0)
    {
        if (!(flags & PM_APP_BG_RUN))
        {
            console_active(pid);
        }
    }
}
