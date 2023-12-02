/**
 * @file scheduler.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-09-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "scheduler.h"
#include "util.h"
#include "assert.h"
#include "thread.h"
#include "init.h"
static scheduler_t scheduler;
umword_t sched_reset = 0;

void scheduler_reset(void)
{
    scheduler_t *sched = scheduler_get_current();

    sched_reset = 0;
    sched->cur_sche = NULL;
}

scheduler_t *scheduler_get_current(void)
{
    return &scheduler;
}

void scheduler_init(void)
{
    for (int i = 0; i < PRIO_MAX; i++)
    {
        slist_init(&(scheduler.prio_list[i]));
    }
}
INIT_HIGH_HAD(scheduler_init);
void scheduler_add(sched_t *node)
{
    thread_t *node_th = container_of(node, thread_t, sche);

    assert(node_th->magic == THREAD_MAGIC);

    scheduler_t *sched = scheduler_get_current();
    assert(node->prio >= 0);
    assert(node->prio < PRIO_MAX);

    if (node->prio > sched->max_prio)
    {
        sched->max_prio = node->prio;
        sched->cur_sche = NULL;
    }

    MK_SET_BIT(sched->bitmap[node->prio / PRIO_MAX], node->prio % PRIO_MAX);

    slist_add(&(sched->prio_list[node->prio]), &node->node);
}
void scheduler_del(sched_t *node)
{
    scheduler_t *sched = scheduler_get_current();
    thread_t *cur_th = thread_get_current();

    assert(node->prio >= 0);
    assert(node->prio < PRIO_MAX);
    slist_del(&node->node);
    // slist_init(&node->node);
    // if (node == &cur_th->sche)
    // { // 删除的是当前的，重新调度
    //     sched->cur_sche = NULL;
    // }
    if (slist_is_empty(&sched->prio_list[node->prio]))
    {
        MK_CLR_BIT(sched->bitmap[node->prio / PRIO_MAX], node->prio % PRIO_MAX);
        for (mword_t i = (PRIO_MAX / WORD_BITS - 1); i >= 0; i--)
        {
            int ret = clz(sched->bitmap[i]);
            if (ret != WORD_BITS)
            {
                int max_prio = (i + 1) * WORD_BITS - ret - 1;
                assert(!slist_is_empty(&sched->prio_list[max_prio]));
                sched->max_prio = max_prio;
                break;
            }
        }
        sched->cur_sche = NULL;
    }
}
sched_t *scheduler_next(void)
{
    scheduler_t *sche = scheduler_get_current();
    sched_t *next_sch = NULL;
    slist_head_t *next = NULL;

    if (sche->cur_sche == NULL)
    {
        next = slist_first(&sche->prio_list[sche->max_prio]);
    }
    else
    {
        next = sche->cur_sche->node.next;
        if (next == &(sche->prio_list[sche->max_prio]))
        {
            next = slist_first(&sche->prio_list[sche->max_prio]);
        }
    }
    next_sch = container_of(next, sched_t, node);
    assert(next_sch->prio < PRIO_MAX);
    sche->cur_sche = next_sch;
    return next_sch;
}

sp_info_t *schde_to(void *usp, void *ksp, umword_t sp_type)
{
    scheduler_t *sche = scheduler_get_current();

    sched_t *next = sche->cur_sche;
    thread_t *next_th = container_of(next, thread_t, sche);

    assert(next_th->magic == THREAD_MAGIC);

    if (sched_reset)
    {
        thread_t *cur_th = thread_get_current();
        cur_th->sp.knl_sp = ksp;
        cur_th->sp.user_sp = usp;
        cur_th->sp.sp_type = sp_type;
    }
    sched_reset = 1;
    return &next_th->sp;
}