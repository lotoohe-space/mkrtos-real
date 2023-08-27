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

/**
 * @brief 线程的初始化函数
 *
 * @param th
 */
void thread_init(thread_t *th)
{
    kobject_init(&th->kobj);
    sched_init(&th->sche);
}
/**
 * @brief 设置运行寄存器
 *
 * @param th
 * @param pc
 * @param ip
 */
void thread_set_exc_regs(thread_t *th, umword_t pc)
{
    pf_t *th_pf = thread_get_pf(th);

    th_pf->pf_s.pc = pc;
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
    to_sche();
}
/**
 * @brief 进行一次调度
 *
 * @param th
 */
void thread_sched(void)
{
    sched_t *next_sche = scheduler_next();

    if (next_sche == &thread_get_current()->sche)
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
    assert(!slist_in_list(&th->sche.node));
    scheduler_add(&th->sche);
    th->status = THREAD_READY;
    if (is_sche)
    {
        to_sche();
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
    thread_init(th);
    return th;
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
    if (kobj)
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
