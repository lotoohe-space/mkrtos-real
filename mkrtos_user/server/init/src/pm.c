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
#include "u_task.h"
#include "u_factory.h"
#include "u_sema.h"
#include "u_thread_util.h"
#include <errno.h>
#include <malloc.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
static pm_t pm;

#define CONS_STACK_SIZE 2048
static ATTR_ALIGN(8) uint8_t cons_stack[CONS_STACK_SIZE];
static uint8_t cons_ipc_msg[MSG_BUG_LEN];
static obj_handler_t cons_th;
static int kill_pid;
static obj_handler_t sem_kill_pid;
static void pm_dispose_func(void)
{
    while (1)
    {
        u_sema_down(sem_kill_pid, 0, NULL);
        pm_rpc_kill_task(0, kill_pid, KILL_SIG, 0);
    }
}
void pm_init(void)
{
    assert(pm_svr_obj_init(&pm) >= 0);
    meta_reg_svr_obj(&pm.svr_obj, PM_PROT);
    sem_kill_pid = handler_alloc();
    assert(sem_kill_pid != HANDLER_INVALID);
    assert(
        msg_tag_get_val(
            u_facotry_create_sema(FACTORY_PROT,
                                  vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, sem_kill_pid), 0, 1)) >= 0);
    u_thread_create(&cons_th, (char *)cons_stack + sizeof(cons_stack) - 8, cons_ipc_msg, pm_dispose_func);
    u_thread_run(cons_th, 4);
    // printf("pm runing..\n");
}
void pm_lock(void)
{
    u_mutex_lock(&pm.lock, 0, NULL);
}
void pm_unlock(void)
{
    u_mutex_unlock(&pm.lock);
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
    msg_tag_t tag = u_task_obj_valid(TASK_THIS, pm_pid2hd(pid), &obj_type);
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
static watch_entry_t *pm_watch_lookup(pm_t *pm, pid_t src_pid, pid_t listen_pid)
{
    watch_entry_t *pos;

    slist_foreach_not_next(pos, &pm->watch_head, node)
    {
        watch_entry_t *next = slist_next_entry(pos, &pm->watch_head, node);
        if (pos->src_pid == src_pid && pos->watch_pid == listen_pid)
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
static void pm_del_watch_by_pid(pm_t *pm, pid_t pid)
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
 * @brief 某个task去另一个task的状态
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
    pm_lock();
    if (pm_watch_lookup(pm, src_pid, pid))
    {
        pm_unlock();
        handler_free_umap(sig_rcv_hd);
        printf("[pm] watch pid:%d, sig hd:%d. failed.\n", pid, sig_rcv_hd);
        fflush(stdout);
        return -EEXIST;
    }
    watch_entry_t *entry = (watch_entry_t *)u_malloc(sizeof(watch_entry_t));

    if (!entry)
    {
        pm_unlock();
        handler_free_umap(sig_rcv_hd);
        return -ENOMEM;
    }
#if 0
    entry->notify_sem_hd = HANDLER_INVALID;
#endif
    entry->sig_hd = sig_rcv_hd;
    entry->src_pid = src_pid; // 监控的pid
    entry->watch_pid = pid;   // 被监控的pid
    entry->flags = flags;
    slist_init(&entry->node);
    slist_add_append(&pm->watch_head, &entry->node);
    pm_unlock();
    printf("[pm] watch pid:%d, sig hd:%d.\n", pid, sig_rcv_hd);
    fflush(stdout);
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
static void pm_send_sig_to_task(pm_t *pm, pid_t pid, umword_t sig_val)
{
    ipc_msg_t *ipc;
    watch_entry_t *pos;
    int ret;

    ipc = thread_get_cur_ipc_msg();
    assert(ipc);

    slist_foreach_not_next(pos, &pm->watch_head, node)
    {
        watch_entry_t *next = slist_next_entry(pos, &pm->watch_head, node);

        // printf("watch_pid:%d pid:%d\n", pos->watch_pid, pid);
        if (pos->watch_pid == pid)
        {
            if (sig_val == KILL_SIG)
            {
                ret = sig_kill(pos->sig_hd, sig_val, pid);
                if (ret < 0)
                {
                    printf("sig kill send failed. sig_hd:%d\n", pos->sig_hd);
                    fflush(stdout);
                }
            }
            slist_del(&pos->node);
            handler_free_umap(pos->sig_hd); //!< 删除信号通知的ipc
            // handler_free_umap(pos->watch_pid); //!< 删除被watch的进程
            u_free(pos);
        }
        pos = next;
    }
    return;
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
    if (src_pid == -1)
    {
        kill_pid = pid;
        u_sema_up(sem_kill_pid);
        return 0;
    }
    if (pid == TASK_THIS)
    {
        printf("not kill init task.\n");
        fflush(stdout);
        return -EINVAL;
    }
    if (pm_pid_is_task(pid) == FALSE)
    {
        printf("pid is error.\n");
        fflush(stdout);
        return -EINVAL;
    }
    pm_lock();
    fs_ns_del_file_by_pid("/", pid); //!< 从ns中删除
#if IS_ENABLED(CONFIG_USING_SIG)
    if (src_pid != pid)
    {
        // 发起者自己删除
        handler_del_umap(pm_pid2hd(pid));
    }
    pm_send_sig_to_task(&pm, pid, KILL_SIG); //!< 给watch者发送sig
#endif
    pm_del_watch_by_pid(&pm, pid); //!< 从watch中删除
    pm_unlock();
    printf("[pm] kill pid:%d code:%d.\n", pid, exit_code);
    fflush(stdout);
    return 0;
}

/**
 * @return >0 pid <0 错误码
 */
static int pm_rpc_create_dummy_task(int mem_block, size_t app_size)
{
    obj_handler_t hd_task = handler_alloc();
    msg_tag_t tag;
    addr_t ram_base;

    if (hd_task == HANDLER_INVALID)
    {
        goto end;
    }
    tag = u_factory_create_task(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, hd_task));
    if (msg_tag_get_prot(tag) < 0)
    {
        goto end_del_obj;
    }
    tag = u_task_alloc_ram_base(hd_task, app_size,
                                &ram_base, mem_block, (addr_t)NULL, 0);
    if (msg_tag_get_prot(tag) < 0)
    {
        goto end_del_obj;
    }
    tag = u_task_set_pid(hd_task, hd_task); //!< 设置进程的pid就是进程hd号码
    if (msg_tag_get_prot(tag) < 0)
    {
        goto end_del_obj;
    }
    printf("dummy task pid:%d addr:0x%x\n\n", hd_task, ram_base);
    return hd_task;
end_del_obj:
    if (hd_task != HANDLER_INVALID)
    {
        u_task_unmap(TASK_THIS, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, hd_task));
    }
end:
    return -ENOMEM;
}
/**
 * @brief 运行一个新的app
 *
 * @param path
 * @param flags
 * @return int
 */
int pm_rpc_run_app(const char *path, pm_flags_t pm_flags, char *params, int params_len_or_app_size,
                   char *envs_in, int envs_in_len)
{
    pid_t pid = HANDLER_INVALID;
    int ret;
    int i;
    int j = 0;
    int args_len = 0;
    int evns_len = 0;

    if (pm_flags.flags & PM_CREATE_DUMMY_TASK)
    {
        return pm_rpc_create_dummy_task(pm_flags.mem_block, params_len_or_app_size);
    }
    int obj_type;
    if (msg_tag_get_val(u_task_obj_valid(TASK_THIS, pm_flags.pid, &obj_type)) != 1)
    {
        pid = HANDLER_INVALID;
    }
    else if (obj_type != TASK_TYPE)
    {
        pid = HANDLER_INVALID;
    }
    else
    {
        pid = pm_flags.pid;
    }
    printf("pm run %s.\n", path);
    char *args[CMD_PARAMS_CN] = {
        (char *)path,
    };
    char *envs[CMD_ENVS_CN] = {};

    for (i = 1, j = 0; *params && i < CMD_PARAMS_CN; i++)
    {
        if (j >= params_len_or_app_size)
        {
            break;
        }
        args[i] = params;
        printf("params[%d]: %s\n", i, params);
        j += strlen(params) + 1;
        params += strlen(params) + 1;
    }
    args_len = i;

    for (i = 0, j = 0; *envs_in && i < CMD_ENVS_CN; i++)
    {
        if (j >= envs_in_len)
        {
            break;
        }
        envs[i] = envs_in;
        printf("envs[%d]: %s\n", i, envs_in);
        j += strlen(envs_in) + 1;
        envs_in += strlen(envs_in) + 1;
    }
    evns_len = i;

    ret = app_load(path, u_get_global_env(), &pid, args, args_len,
                   envs, evns_len, pm_flags.mem_block,
                   !!(pm_flags.flags & PM_USE_LOAD_TO_RAM));
    if (ret >= 0)
    {
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

    tag = u_task_copy_data_to(pm_hd2pid(src_pid), pm_hd2pid(dst_pid),
                              (void *)src_addr, (void *)dst_addr, len);

    return msg_tag_get_val(tag);
}
