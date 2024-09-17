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
#include "assert.h"
#include "init.h"
#include "thread.h"
#include "util.h"
#include <arch.h>
#include <pre_cpu.h>
PER_CPU(scheduler_t, scheduler);

void scheduler_reset(void)
{
    scheduler_t *sched = scheduler_get_current();

    sched->sched_reset = 0;
    sched->cur_sche = NULL;
#if SCHEDULER_DEBUG
    printk("%s set to null\n", __func__);
#endif
}

scheduler_t *scheduler_get_current(void)
{
    return pre_cpu_get_current_cpu_var(&scheduler);
}
scheduler_t *scheduler_get_cpu(int inx)
{
    return pre_cpu_get_var_cpu(inx, &scheduler);
}
void scheduler_init(void)
{
    for (int j = 0; j < CONFIG_CPU; j++)
    {
        for (int i = 0; i < PRIO_MAX; i++)
        {
            slist_init(&(scheduler_get_cpu(j)->prio_list[i]));
        }
    }
}
INIT_KOBJ(scheduler_init);
bool_t scheduler_add(sched_t *node)
{
    return scheduler_add_to_cpu(node, arch_get_current_cpu_id());
}
bool_t scheduler_add_to_cpu(sched_t *node, int cpu)
{
    int ret = 0;
    thread_t *node_th = container_of(node, thread_t, sche);

    assert(cpulock_get_status());

    assert(node_th->magic == THREAD_MAGIC);

    scheduler_t *sched = scheduler_get_cpu(cpu);
    assert(node->prio >= 0);
    assert(node->prio < PRIO_MAX);

    if (node->prio > sched->max_prio)
    {
        sched->max_prio = node->prio;
        sched->cur_sche = NULL;
#if SCHEDULER_DEBUG
        printk("%s set to null\n", __func__);
#endif
        ret = 1;
    }

    MK_SET_BIT(sched->bitmap[node->prio / PRIO_MAX], node->prio % PRIO_MAX);

    slist_add(&(sched->prio_list[node->prio]), &node->node);
    return ret;
}
void scheduler_del(sched_t *node)
{
    scheduler_t *sched = scheduler_get_current();
    thread_t *th = container_of(node, thread_t, sche);

    assert(cpulock_get_status());
    assert(thread_get_status(th) == THREAD_SUSPEND);
    assert(node->prio >= 0);
    assert(node->prio < PRIO_MAX);
    slist_del(&node->node);

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
#if SCHEDULER_DEBUG
        printk("%s set to null\n", __func__);
#endif
    }
}
sched_t *scheduler_next_cpu(int cpu)
{
    scheduler_t *sche = scheduler_get_cpu(cpu);
    sched_t *next_sch = NULL;
    slist_head_t *next = NULL;
    assert(cpulock_get_status());

    if (sche->cur_sche == NULL || slist_is_empty(&sche->cur_sche->node))
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
sched_t *scheduler_next(void)
{
    return scheduler_next_cpu(arch_get_current_cpu_id());
}

void sched_tail(void)
{
    sti();
}
