/*
 * @Author: zhangzheng 1358745329@qq.com
 * @Date: 2023-08-18 15:03:16
 * @LastEditors: zhangzheng 1358745329@qq.com
 * @LastEditTime: 2023-08-18 15:50:15
 * @FilePath: /mkrtos-real/mkrtos_knl/inc/knl/scheduler.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include "slist.h"

#define PRIO_MAX 32

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
    slist_head_t prio_list[PRIO_MAX];
    sched_t *cur_sche;
    int max_prio;
} scheduler_t;

scheduler_t *scheduler_get_current(void);
void scheduler_init(void);
sched_t *scheduler_next(void);
void scheduler_add(sched_t *node);
void scheduler_del(sched_t *node);
