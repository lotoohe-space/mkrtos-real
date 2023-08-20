/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2023-08-14 09:47:54
 * @LastEditors: zhangzheng 1358745329@qq.com
 * @LastEditTime: 2023-08-18 15:50:09
 * @FilePath: /mkrtos-real/mkrtos_knl/knl/scheduler.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "scheduler.h"
#include "util.h"
#include "assert.h"
#include "thread.h"
#include "init.h"
static scheduler_t scheduler;

scheduler_t *scheduler_get_current(void)
{
    return &scheduler;
}

INIT_HIGH_HAD void scheduler_init(void)
{
    for (int i = 0; i < PRIO_MAX; i++)
    {
        slist_init(&scheduler.prio_list[i]);
    }
}
void scheduler_add(scheduler_t *sched, sched_t *node)
{
    assert(node->prio >= 0);
    assert(node->prio < PRIO_MAX);

    slist_add(&sched->prio_list[node->prio], &node->node);
}
void scheduler_del(sched_t *node)
{
    assert(node->prio >= 0);
    assert(node->prio < PRIO_MAX);
    slist_del(&node->node);
}
sched_t *scheduler_next(sched_t *cur)
{
    scheduler_t *sche = scheduler_get_current();
    sched_t *next_sch = NULL;
    slist_head_t *next = NULL;

    if (cur == NULL)
    {
        next = slist_first(&sche->prio_list[sche->max_prio]);
    }
    else
    {
        next = cur->node.next;
        if (next == &sche->prio_list[sche->max_prio])
        {
            next = slist_first(&sche->prio_list[sche->max_prio]);
        }
    }
    next_sch = container_of(next, sched_t, node);
    sche->cur_sche = next_sch;
    return next_sch;
}

sp_info_t *schde_to(void *sp, umword_t sp_type)
{
    scheduler_t *sche = scheduler_get_current();

    thread_t *cur_th = thread_get_current();
    sched_t *next = sche->cur_sche;
    thread_t *next_th = container_of(next, thread_t, sche);

    if (!sp_type)
    {
        cur_th->sp.knl_sp = sp;
    }
    else
    {
        cur_th->sp.user_sp = sp;
    }
    cur_th->sp.sp_type = sp_type;
    return &next_th->sp;
}