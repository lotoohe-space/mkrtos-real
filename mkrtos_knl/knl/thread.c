/*
 * @Author: zhangzheng 1358745329@qq.com
 * @Date: 2023-08-14 09:47:54
 * @LastEditors: zhangzheng 1358745329@qq.com
 * @LastEditTime: 2023-08-18 16:21:20
 * @FilePath: /mkrtos-real/mkrtos_knl/knl/thread.c
 * @Description: 线程管理相关
 */

#include "types.h"
#include "kobject.h"
#include "ram_limit.h"
#include "factory.h"
#include "thread.h"
#include "mm_wrap.h"
#include "string.h"
#include "init.h"
#include "task.h"
#include "thread.h"
#include "slist.h"
#include "thread_armv7m.h"
#include "assert.h"

static msg_tag_t
thread_syscall(kobject_t *kobj, ram_limit_t *ram, entry_frame_t *f);
static bool_t thread_put(kobject_t *kobj);
static void thread_release_stage1(kobject_t *kobj);
static void thread_release_stage2(kobject_t *kobj);
/**
 * @brief 线程的初始化函数
 *
 * @param th
 */
void thread_init(thread_t *th, ram_limit_t *lim)
{
    kobject_init(&th->kobj);
    sched_init(&th->sche);
    slist_init(&th->wait);
    ref_counter_init(&th->ref);
    ref_counter_inc(&th->ref);
    th->lim = lim;
    th->kobj.invoke_func = thread_syscall;
    th->kobj.put_func = thread_put;
    th->kobj.stage_1_func = thread_release_stage1;
    th->kobj.stage_2_func = thread_release_stage2;
    th->magic = THREAD_MAIGC;
}
static bool_t thread_put(kobject_t *kobj)
{
    thread_t *th = container_of(kobj, thread_t, kobj);

    return ref_counter_dec(&th->ref) == 1;
}
static void thread_release_stage1(kobject_t *kobj)
{
    thread_t *th = container_of(kobj, thread_t, kobj);
    thread_suspend(th);
    thread_unbind(th);
}
static void thread_release_stage2(kobject_t *kobj)
{
    thread_t *th = container_of(kobj, thread_t, kobj);
    thread_t *cur_th = thread_get_current();

    mm_limit_free_align(th->lim, kobj, THREAD_BLOCK_SIZE);

    if (cur_th == th)
    {
        thread_sched();
    }
}

/**
 * @brief 设置运行寄存器
 *
 * @param th
 * @param pc
 * @param ip
 */
void thread_set_exc_regs(thread_t *th, umword_t pc, umword_t user_sp, umword_t ram)
{
    thread_user_pf_set(th, (void *)pc, (void *)user_sp, (void *)ram);
}
/**
 * @brief 线程绑定到task
 *
 * @param th
 * @param tk
 */
void thread_bind(thread_t *th, kobject_t *tk)
{
    th->task = tk;
    task_t *tsk = container_of(tk, task_t, kobj);

    ref_counter_inc(&tsk->ref_cn);
}
/**
 * @brief 解除task绑定
 *
 * @param th
 */
void thread_unbind(thread_t *th)
{
    task_t *tsk = container_of(th->task, task_t, kobj);

    ref_counter_dec(&tsk->ref_cn);
    th->task = NULL;
}
/**
 * @brief 挂起一个线程
 *
 * @param th
 */
void thread_suspend(thread_t *th)
{
    assert(slist_in_list(&th->sche.node));
    scheduler_del(&th->sche);
    th->status = THREAD_SUSPEND;
    thread_sched();
}
/**
 * @brief 进行一次调度
 *
 * @param th
 */
void thread_sched(void)
{
    sched_t *next_sche = scheduler_next();
    thread_t *th = thread_get_current();

    assert(th->magic == THREAD_MAIGC);
    if (next_sche == &th->sche)
    {
        return;
    }
    to_sche();
}
/**
 * @brief 线程进入就绪态
 *
 * @param th
 */
void thread_ready(thread_t *th, bool_t is_sche)
{
    if (!!slist_in_list(&th->sche.node))
    {
        assert(!slist_in_list(&th->sche.node));
    }
    scheduler_add(&th->sche);
    th->status = THREAD_READY;
    if (is_sche)
    {
        thread_sched();
    }
}
/**
 * @brief 创建线程
 *
 * @param ram
 * @return thread_t*
 */
thread_t *thread_create(ram_limit_t *ram)
{
    thread_t *th = mm_limit_alloc_align(ram, THREAD_BLOCK_SIZE, THREAD_BLOCK_SIZE);

    if (!th)
    {
        return NULL;
    }
    memset(th, 0, THREAD_BLOCK_SIZE);
    thread_init(th, ram);
    return th;
}
enum thread_op
{
    SET_EXEC_REGS,
    RUN_THREAD,
    BIND_TASK,
    MSG_BUG_GET,
    MSG_BUG_SET,
};
static msg_tag_t
thread_syscall(kobject_t *kobj, ram_limit_t *ram, entry_frame_t *f)
{
    msg_tag_t tag = msg_tag_init(f->r[0]);
    task_t *task = thread_get_current_task();
    thread_t *tag_th = container_of(kobj, thread_t, kobj);
    thread_t *cur_th = thread_get_current();

    if (tag.prot != THREAD_PROT)
    {
        return msg_tag_init3(0, 0, -EPROTO);
    }

    switch (tag.type)
    {
    case SET_EXEC_REGS:
    {
        thread_set_exc_regs(tag_th, f->r[1], f->r[2], f->r[3]);
    }
    break;
    case MSG_BUG_SET:
    {
        /*TODO:检查内存的可访问性*/
        thread_set_msg_bug(tag_th, (void *)(f->r[1]), THREAD_MSG_BUG_LEN);
        tag = msg_tag_init3(0, 0, 0);
    }
    case MSG_BUG_GET:
    {
        f->r[1] = (umword_t)(tag_th->msg.msg);
        f->r[2] = THREAD_MSG_BUG_LEN;
        if (tag_th->msg.msg == NULL)
        {
            tag = msg_tag_init3(0, 0, -EACCES);
        }
        else
        {
            tag = msg_tag_init3(0, 0, 0);
        }
    }
    break;
    case RUN_THREAD:
    {
        if (!slist_in_list(&tag_th->sche.node))
        {
            thread_ready(tag_th, TRUE);
        }
    }
    break;
    case BIND_TASK:
    {
        kobject_t *task_kobj = obj_space_lookup_kobj(&task->obj_space, f->r[1]);
        if (task_kobj == NULL /*TODO:检测kobj类型*/)
        {
            tag = msg_tag_init3(0, 0, -ENOENT);
            return tag;
        }
        thread_bind(tag_th, task_kobj);
        printk("thread bind to %d\n", f->r[7], f->r[1]);
    }
    break;
    }
    return tag;
}

/**
 * @brief 该函数创建一个工厂对象
 *
 * @param lim
 * @param arg0
 * @param arg1
 * @param arg2
 * @param arg3
 * @return kobject_t*
 */
static kobject_t *thread_create_func(ram_limit_t *lim, umword_t arg0, umword_t arg1,
                                     umword_t arg2, umword_t arg3)
{
    kobject_t *kobj = (kobject_t *)thread_create(lim);
    if (!kobj)
    {
        return NULL;
    }
    return kobj;
}

/**
 * @brief 工厂注册函数
 *
 */
void thread_factory_register(void)
{
    factory_register(thread_create_func, THREAD_PROT);
}
INIT_KOBJ(thread_factory_register);
/**
 * @brief 获取当前的task
 *
 * @return task_t*
 */
task_t *thread_get_current_task(void)
{
    return container_of(
        thread_get_current()->task, task_t, kobj);
}
