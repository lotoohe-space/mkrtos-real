/**
 * @file sys_backend.c
 * @author zhangzheng (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-11-19
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "u_ipc.h"
#include "u_factory.h"
#include "u_thread.h"
#include "u_task.h"
#include "u_hd_man.h"
#include "u_log.h"
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
    umword_t *stack_tmp = (umword_t *)stack;

    *(--stack_tmp) = (umword_t)args;
    *(--stack_tmp) = (umword_t)0;
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
    thread_run(th1_hd, 2);

    return 0;
}
