/**
 * @file task.h
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-11-22
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include "kobject.h"
#include "slist.h"
#include "mm_wrap.h"
#include "assert.h"
#include "kobject.h"
#include "obj_space.h"
#include "mm_space.h"
#include "ref.h"

typedef struct task
{
    kobject_t kobj;        //!< task kernel object.
    obj_space_t obj_space; //!< object space.
    mm_space_t mm_space;   //!< memory space.
    ram_limit_t *lim;      //!< ram use limit.
    kobject_t *exec_th;    //!< execption thread.
    ref_counter_t ref_cn;  //!< ref count.
    slist_head_t del_node; //!< delect list node.
    void *nofity_point;    //!< commint point.
    addr_t nofity_stack;   //!< nofity_point_stack.
    pid_t pid;             //!< task pid.
} task_t;

static inline pid_t task_pid_get(task_t *task)
{
    return task->pid;
}
void task_init(task_t *task, ram_limit_t *ram, int is_knl);
task_t *task_create(ram_limit_t *lim, int is_knl);
int task_alloc_base_ram(task_t *tk, ram_limit_t *lim, size_t size);
void task_kill(task_t *tk);
int task_set_pid(task_t *task, pid_t pid);
