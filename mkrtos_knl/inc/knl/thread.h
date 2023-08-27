/*
 * @Author: zhangzheng 1358745329@qq.com
 * @Date: 2023-08-14 09:47:54
 * @LastEditors: zhangzheng 1358745329@qq.com
 * @LastEditTime: 2023-08-18 16:21:48
 * @FilePath: /mkrtos-real/mkrtos_knl/inc/knl/thread.h
 * @Description: 线程管理相关
 */
#pragma once

#include "kobject.h"
#include "scheduler.h"
#include "util.h"
#include "arch.h"

#define THREAD_BLOCK_SIZE 0x200 //!< 线程块大小，栈在块的顶部

enum thread_state
{
    THREAD_IDLE,
    THREAD_DEAD,
    THREAD_SUSPEND,
    THREAD_READY,
};
typedef struct
{
    umword_t rg0[4]; //!< r0-r3
    umword_t r12;
    umword_t lr;
    umword_t pc;
    umword_t xpsr;
} pf_s_t;
typedef struct pf
{
    umword_t rg1[8]; //!< r4-r11
    pf_s_t pf_s;
} pf_t;

typedef struct sp_info
{
    void *user_sp;   //!< 用户态的sp
    void *knl_sp;    //!< 内核sp
    mword_t sp_type; //!< 使用的栈类型
} sp_info_t;

typedef struct thread
{
    kobject_t kobj;           //!< 内核对象节点
    sched_t sche;             //!< 调度节点
    enum thread_state status; //!< 线程状态
    kobject_t *task;          //!< 绑定的task
    sp_info_t sp;             //!< sp信息
} thread_t;

static inline enum thread_state thread_get_status(thread_t *th)
{
    return th->status;
}
static inline pf_t *thread_get_pf(thread_t *th)
{
    uint8_t *bottom = (uint8_t *)th;

    return ((pf_t *)(bottom + THREAD_BLOCK_SIZE)) - 1;
}

static inline thread_t *thread_get_current(void)
{
    umword_t sp = arch_get_sp();

    return (thread_t *)(ALIGN_DOWN(sp, THREAD_BLOCK_SIZE));
}

static inline pf_t *thread_get_current_pf(void)
{
    return thread_get_pf(thread_get_current());
}
void thread_init(thread_t *th);
void thread_set_exc_regs(thread_t *th, umword_t pc);
thread_t *thread_create(ram_limit_t *ram);
void thread_bind(thread_t *th, kobject_t *tk);

void thread_sched(void);
void thread_suspend(thread_t *th);
void thread_ready(thread_t *th, bool_t is_sche);