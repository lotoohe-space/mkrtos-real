/**
 * @file ipc.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-09-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "ipc.h"
#include "types.h"
#include "init.h"
#include "prot.h"
#include "kobject.h"
#include "factory.h"
#include "task.h"
#include "thread.h"
#include "assert.h"
#include "slist.h"
#include "spinlock.h"
#include "string.h"
#include "mm_wrap.h"
#include "map.h"

enum ipc_op
{
    IPC_BIND,
    IPC_DO = 6,
};
typedef struct ipc_wait_bind_entry
{
    slist_head_t node;
    thread_t *th;
} ipc_wait_bind_entry_t;

#if IS_ENABLED(CONFIG_BUDDY_SLAB)
#include <slab.h>
slab_t *ipc_slab;
#endif

/**
 * @brief 在系统初始化时调用，初始化task的内存
 *
 */
static void ipc_obj_slab_init(void)
{
#if IS_ENABLED(CONFIG_BUDDY_SLAB)
    ipc_slab = slab_create(sizeof(ipc_t), "ipc");
    assert(ipc_slab);
#endif
}
INIT_KOBJ_MEM(ipc_obj_slab_init);

int ipc_bind(ipc_t *ipc, obj_handler_t th_hd, umword_t user_id, thread_t *th_kobj)
{
    int ret = -EINVAL;
    task_t *cur_task = thread_get_current_task();

    /*TODO:原子操作，绑定其他线程不一定是当前线程*/
    if (ipc->svr_th == NULL)
    {
        mword_t status = spinlock_lock(&cur_task->kobj.lock); //!< 锁住当前的task

        if (status < 0)
        {
            return -EACCES;
        }
        ref_counter_inc(&cur_task->ref_cn); //!< task引用计数+1
        thread_t *recv_kobj;

        if (!th_kobj)
        {
            recv_kobj = (thread_t *)obj_space_lookup_kobj_cmp_type(&cur_task->obj_space, th_hd, THREAD_TYPE);
        }
        else
        {
            recv_kobj = th_kobj;
        }
        if (!recv_kobj)
        {
            ret = -ENOENT;
            goto end_bind;
        }
        ref_counter_inc(&recv_kobj->ref); //!< 绑定后线程的引用计数+1，防止被删除
        ipc->svr_th = recv_kobj;
        ipc->user_id = user_id;
        ipc_wait_bind_entry_t *pos;

        slist_foreach_not_next(pos, &ipc->wait_bind, node) //!< 唤醒所有等待绑定的线程
        {
            ipc_wait_bind_entry_t *next = slist_next_entry(pos, &ipc->wait_bind, node);
            assert(pos->th->status == THREAD_SUSPEND);
            slist_del(&next->node);
            thread_ready_remote(pos->th, TRUE);
            pos = next;
        }
        ret = 0;
    end_bind:
        //!< 先解锁，然后在给task的引用计数-1
        spinlock_set(&cur_task->kobj.lock, status);
        ref_counter_dec_and_release(&cur_task->ref_cn, &cur_task->kobj);
    }
    else
    {
        ret = -ECANCELED;
    }
    return ret;
}
/**
 * @brief ipc的系统调用
 *
 * @param kobj
 * @param ram
 * @param f
 * @return msg_tag_t
 */
static void ipc_syscall(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f)
{
    assert(kobj);
    assert(f);
    msg_tag_t tag = msg_tag_init4(0, 0, 0, -EINVAL);
    thread_t *th = thread_get_current();
    task_t *cur_task = thread_get_current_task();
    ipc_t *ipc = container_of(kobj, ipc_t, kobj);

    if (sys_p.prot != IPC_PROT && sys_p.prot != THREAD_PROT)
    {
        //!< ipc对象拥有代理thread消息的功能，所以这里对与thread协议进行放宽
        f->regs[0] = msg_tag_init4(0, 0, 0, -EPROTO).raw;
        return;
    }
    switch (sys_p.op)
    {
    case IPC_BIND:
    {
        int ret = 0;
        if (sys_p.prot == THREAD_PROT)
        {
            tag = msg_tag_init4(0, 0, 0, -EPROTO);
            break;
        }
        ret = ipc_bind(ipc, f->regs[0], f->regs[1], NULL);
        tag = msg_tag_init4(0, 0, 0, ret);
    }
    break;
    case IPC_DO:
    {
        //!< 如果是ipc协议，则当成一个ipc处理
        if (ipc->svr_th == th)
        {
            tag = msg_tag_init4(0, 0, 0, -ECANCELED);
            break;
        }
    again:
        if (ipc->svr_th == NULL)
        {
            ipc_wait_bind_entry_t entry = {
                .th = th,
            };
            slist_init(&entry.node);
            mword_t status = spinlock_lock(&ipc->lock);

            slist_add(&ipc->wait_bind, &entry.node);
            thread_suspend(th);
            preemption();
            slist_del(&entry.node);
            if (th->ipc_status == THREAD_IPC_ABORT)
            {
                th->ipc_status = THREAD_NONE;
                tag = msg_tag_init4(0, 0, 0, -ENOENT);
            }

            spinlock_set(&ipc->lock, status);
            goto again;
        }
        else
        {
            tag = thread_do_ipc(&ipc->svr_th->kobj, f, ipc->user_id);
        }
    }
    break;
    default:
        break;
    }
    f->regs[0] = tag.raw;
}
static void ipc_release_stage1(kobject_t *kobj)
{
    ipc_t *ipc = container_of(kobj, ipc_t, kobj);

    kobject_invalidate(kobj);
    ipc_wait_bind_entry_t *pos;

    slist_foreach(pos, &ipc->wait_bind, node)
    {
        assert(pos->th->status == THREAD_SUSPEND);
        pos->th->ipc_status == THREAD_IPC_ABORT;
        thread_ready_remote(pos->th, TRUE);
    }
    if (ipc->svr_th)
    {
        ref_counter_dec_and_release(&ipc->svr_th->ref, &ipc->svr_th->kobj);
        ipc->svr_th = NULL;
    }
}
static void ipc_release_stage2(kobject_t *kobj)
{
    ipc_t *ipc = container_of(kobj, ipc_t, kobj);

#if IS_ENABLED(CONFIG_BUDDY_SLAB)
    mm_limit_free_slab(ipc_slab, ipc->lim, kobj);
#else
    mm_limit_free(ipc->lim, kobj);
#endif
    // printk("ipc release 0x%x\n", kobj);
}
static bool_t ipc_put(kobject_t *kobj)
{
    ipc_t *ipc = container_of(kobj, ipc_t, kobj);

    return ref_counter_dec(&ipc->ref) == 1;
}
static void ipc_init(ipc_t *ipc, ram_limit_t *lim)
{
    kobject_init(&ipc->kobj, IPC_TYPE);
    slist_init(&ipc->wait_bind);
    spinlock_init(&ipc->lock);
    ref_counter_init(&ipc->ref);
    ref_counter_inc(&ipc->ref);
    ipc->lim = lim;
    ipc->kobj.put_func = ipc_put;
    ipc->kobj.invoke_func = ipc_syscall;
    ipc->kobj.stage_1_func = ipc_release_stage1;
    ipc->kobj.stage_2_func = ipc_release_stage2;
}
static ipc_t *ipc_create(ram_limit_t *lim)
{
    ipc_t *ipc;

#if IS_ENABLED(CONFIG_BUDDY_SLAB)
    ipc = mm_limit_alloc_slab(ipc_slab, lim);
#else  
    ipc = mm_limit_alloc(lim, sizeof(ipc_t));
#endif
    if (!ipc)
    {
        return NULL;
    }
    ipc_init(ipc, lim);
    return ipc;
}
static kobject_t *ipc_create_func(ram_limit_t *lim, umword_t arg0, umword_t arg1,
                                  umword_t arg2, umword_t arg3)
{

    return &ipc_create(lim)->kobj;
}

/**
 * @brief 工厂注册函数
 *
 */
static void ipc_factory_register(void)
{
    factory_register(ipc_create_func, IPC_PROT);
}
INIT_KOBJ(ipc_factory_register);

void ipc_dump(void)
{
}
