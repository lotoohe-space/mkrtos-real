/**
 * @file sys_backend.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-11-19
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "u_ipc.h"
#include "u_types.h"
#include "u_factory.h"
#include "u_thread.h"
#include "u_task.h"
#include "u_hd_man.h"
#include "u_log.h"
#include "pm_cli.h"
#include "u_sig.h"
#include "futex_queue.h"
#include "u_ipc.h"
#include "u_sys.h"
#include "u_futex.h"
#include <futex.h>
#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <pthread_impl.h>
#include <atomic.h>
static int pthread_cnt = 1;

void pthread_cnt_inc(void)
{
    a_inc(&pthread_cnt);
}
int pthread_cnt_dec(void)
{
    return a_fetch_add(&pthread_cnt, -1);
}
int pthread_get(void)
{
    return pthread_cnt;
}
struct start_args
{
    void *(*start_func)(void *);
    void *start_arg;
    volatile int control;
    unsigned long sig_mask[_NSIG / 8 / sizeof(long)];
    void *tp;
    int start_flag;
};
extern void __pthread_new_thread_entry__(void);
int be_clone(int (*func)(void *), void *stack, int flags, void *args, pid_t *ptid, void *tls, pid_t *ctid)
{
    if (!(flags & 0x10000))
    {
        assert(0);
    }
    msg_tag_t tag;
    obj_handler_t th1_hd;
    struct pthread *ph;
    struct start_args *st_args = args;

    ph = (struct pthread *)((char *)tls - sizeof(struct pthread));

    th1_hd = handler_alloc();
    if (th1_hd == HANDLER_INVALID)
    {
        return -ENOENT;
    }

    tag = factory_create_thread(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, th1_hd));
    if (msg_tag_get_prot(tag) < 0)
    {
        handler_free(th1_hd);
        return msg_tag_get_prot(tag);
    }
    tag = thread_msg_buf_set(th1_hd, stack - MSG_BUG_LEN);
    if (msg_tag_get_prot(tag) < 0)
    {
        handler_free_umap(th1_hd);
        return msg_tag_get_prot(tag);
    }
    stack = (char *)stack - MSG_BUG_LEN;
    stack = (void *)(((umword_t)stack - sizeof(void *)) & (~(sizeof(void *) * 2 - 1)));
    umword_t *stack_tmp = (umword_t *)stack;

    // 设置调用参数等
    *(--stack_tmp) = (umword_t)args;
    *(--stack_tmp) = (umword_t)0; // 保留
    *(--stack_tmp) = (umword_t)func;

    tag = thread_exec_regs(th1_hd, (umword_t)__pthread_new_thread_entry__, (umword_t)stack_tmp, RAM_BASE(), 0);
    if (msg_tag_get_prot(tag) < 0)
    {
        handler_free_umap(th1_hd);
        return msg_tag_get_prot(tag);
    }
    tag = thread_bind_task(th1_hd, TASK_THIS);
    if (msg_tag_get_prot(tag) < 0)
    {
        handler_free_umap(th1_hd);
        return msg_tag_get_prot(tag);
    }
    ph->hd = th1_hd;
    ph->ctid = (umword_t)ctid;
    *ptid = th1_hd;
    pthread_cnt_inc();
    if (!(st_args->start_flag & PTHREAD_DONT_RUN))
    {
        thread_run(th1_hd, 2); // 优先级默认为2
    }

    return 0;
}

