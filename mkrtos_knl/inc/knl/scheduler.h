/*
 * @Author: ATShining 1358745329@qq.com
 * @Date: 2023-08-18 15:03:16
 * @LastEditors: ATShining 1358745329@qq.com
 * @LastEditTime: 2023-08-18 15:50:15
 * @FilePath: /mkrtos-real/mkrtos_knl/inc/knl/scheduler.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include "slist.h"

#define PRIO_MAX WORD_BITS

#define SCHEDULER_DEBUG 0

typedef struct sched
{
    int prio;
    slist_head_t node;
} sched_t;

static void sched_init(sched_t *sche)
{
    slist_init(&sche->node);
    sche->prio = 0;
}

typedef struct scheduler
{
    umword_t sched_reset; // 放到第一个方便汇编操作
    slist_head_t prio_list[PRIO_MAX];
    umword_t bitmap[PRIO_MAX / WORD_BITS];
    sched_t *cur_sche;
    int max_prio; //<!当前优先级最大的队列号
} scheduler_t;

scheduler_t *scheduler_get_current(void);
void scheduler_init(void);
sched_t *scheduler_next(void);
bool_t scheduler_add(sched_t *node);
bool_t scheduler_add_to_cpu(sched_t *node, int cpu);
sched_t *scheduler_next_cpu(int cpu);
void scheduler_del(sched_t *node);
void scheduler_reset(void);
