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
#include "sema.h"
typedef struct task
{
    kobject_t kobj;        //!< task kernel object.
    obj_space_t obj_space; //!< object space.
    mm_space_t mm_space;   //!< memory space.
    ram_limit_t *lim;      //!< ram use limit.
    kobject_t *exec_th;    //!< execption thread.
    ref_counter_t ref_cn;  //!< ref count.
    slist_head_t del_node; //!< delect list node.

    void *notify_point;  //!< commint point func.
    addr_t nofity_stack; //!< notify_point_stack.
    mutex_t nofity_lock;
    sema_t notify_sema;    //!< sema
    addr_t nofity_msg_buf; //!<
    obj_handler_t (*nofity_map_buf)[CONFIG_THREAD_MAP_BUF_LEN];
    umword_t *nofity_bitmap; //!<
    int nofity_bitmap_len;   //!< max is WORD_BITS
    slist_head_t nofity_theads_head;

    void *text_addr; //!< text start addr for cmbacktrace.
    size_t text_size;
    pid_t pid; //!< task pid.
} task_t;
typedef struct
{
    void *com_point_func;
    addr_t stack;
    umword_t stack_size;
    void *bitmap;
    int bitmap_len;
    void *msg_buf;
    obj_handler_t (*map_buf)[CONFIG_THREAD_MAP_BUF_LEN];
} fast_ipc_info_t;
static inline pid_t task_pid_get(task_t *task)
{
    return task->pid;
}
void task_init(task_t *task, ram_limit_t *ram, int is_knl);
task_t *task_create(ram_limit_t *lim, int is_knl);
int task_alloc_base_ram(task_t *tk, ram_limit_t *lim, size_t size, int mem_block);
void task_kill(task_t *tk);
int u_task_set_pid(task_t *task, pid_t pid);
