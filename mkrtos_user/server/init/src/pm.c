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
#include "ns.h"
#include "u_task.h"
#include "u_hd_man.h"
#include "u_sig.h"
#include "pm.h"
#include "parse_cfg.h"
#include "u_malloc.h"
#include "nsfs.h"
#include "sig_cli.h"
#include "tty.h"
#include <errno.h>
#include <malloc.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
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
            u_free(pos);
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
    watch_entry_t *entry = (watch_entry_t *)u_malloc(sizeof(watch_entry_t));

    if (!entry)
    {
        handler_free_umap(sig_rcv_hd);
        return -ENOMEM;
    }
#if 0
    entry->notify_sem_hd = HANDLER_INVALID;
#endif
    entry->sig_hd = sig_rcv_hd;
    entry->src_pid = src_pid;
    entry->watch_pid = pid;
    entry->flags = flags;
    slist_init(&entry->node);
    slist_add_append(&pm->watch_head, &entry->node);
    printf("[pm] watch pid:%d, sig hd:%d.\n", pid, sig_rcv_hd);
    return 0;
}
#if IS_ENABLED(CONFIG_USING_SIG)
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

        printf("watch_pid:%d pid:%d\n", pos->watch_pid, pid);
        if (pos->watch_pid == pid)
        {
            if (sig_val == KILL_SIG)
            {
                int ret;

                ret = sig_kill(pos->sig_hd, sig_val, pid);
            }
            slist_del(&pos->node);
            handler_free_umap(pos->sig_hd);   //!< 删除信号通知的ipc
            handler_free_umap(pos->watch_pid); //!< 删除被watch的进程
            u_free(pos);
        }
        pos = next;
    }
}
#endif
/**
 * @brief 杀死某个进程
 *
 * @param pid
 * @param flags
 * @return int
 */
int pm_rpc_kill_task(int src_pid, int pid, int flags, int exit_code)
{
    if (pid == TASK_THIS)
    {
        printf("not kill init task.\n");
        return -EINVAL;
    }
    if (pm_pid_is_task(pid) == FALSE)
    {
        printf("pid is error.\n");
        return -EINVAL;
    }

    // ns_node_del_by_pid(pid, flags); TODO:         //!< 从ns中删除
    #if IS_ENABLED(CONFIG_USING_SIG)
    if (src_pid != pid)
    {
        // 发起者自己删除
        handler_del_umap(pid);
    }
    pm_send_sig_to_task(&pm, pid, KILL_SIG); //!< 给watch者发送sig
    #endif
    pm_del_watch_by_pid(&pm, pid); //!< 从watch中删除
    printf("[pm] kill pid:%d code:%d.\n", pid, exit_code);
    return 0;
}
/**
 * @brief 运行一个新的app
 *
 * @param path
 * @param flags
 * @return int
 */
int pm_rpc_run_app(const char *path, int flags, char *params, int params_len)
{
    pid_t pid;
    int ret;
    obj_handler_t sem;
    int i;
    int j = 0;
    printf("pm run %s.\n", path);
    char *args[CMD_PARAMS_CN] = {
        (char *)path,
    };

    for (i = 1; *params && i < CMD_PARAMS_CN; i++)
    {
        if (j >= params_len)
        {
            break;
        }
        args[i] = params;
        printf("params[%d]: %s\n", i, params);
        j += strlen(params) + 1;
        params += strlen(params) + 1;
    }

    ret = app_load(path, u_get_global_env(), &pid, args, i,
                   NULL, 0, &sem, 0);
    if (ret >= 0)
    {
        #if 0
        if (!(flags & PM_APP_BG_RUN))
        {
            tty_set_fg_pid(pid);
        }
        #endif
        return pid;
    }
    return ret;
}
/**
 * @brief 用于拷贝数据
 *
 * @param src_pid
 * @param dst_pid
 * @param src_addr
 * @param dst_addr
 * @param len
 * @return int
 */
int pm_rpc_copy_data(pid_t src_pid, pid_t dst_pid, umword_t src_addr, umword_t dst_addr, size_t len)
{
    msg_tag_t tag;

    tag = task_copy_data_to(pm_hd2pid(src_pid), pm_hd2pid(dst_pid),
                            (void *)src_addr, (void *)dst_addr, len);

    return msg_tag_get_val(tag);
}
#if 0
/**
 * @brief 等待某个进程死亡
 */
int pm_waitpid(obj_handler_t sig_rcv_hd, pid_t pid)
{
    pid_t src_pid = thread_get_src_pid();
    watch_entry_t *pos;

    /*TODO:检测sig_rcv_hd的类型*/
    slist_foreach_not_next(pos, &pm->watch_head, node)
    {
        watch_entry_t *next = slist_next_entry(pos, &pm->watch_head, node);

        if (pos->watch_pid == pid && src_pid == pos->src_pid)
        {
            pos->notify_sem_hd = sig_rcv_hd;
            break;
        }
        pos = next;
    }
    return 0;
}
#endif