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
#include "ref.h"
#define THREAD_BLOCK_SIZE 0x400 //!< 线程块大小，栈在块的顶部
struct thread;
typedef struct thread thread_t;
enum thread_state
{
    THREAD_IDLE,
    THREAD_DEAD,
    THREAD_SUSPEND, //!< 只有接收和发送ipc消息时才能挂起
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

#define THREAD_MSG_BUG_LEN 128       //!< 默认的消息寄存器大小
#define MSG_BUF_HAS_DATA_FLAGS 0x01U //!< 已经有数据了
#define MSG_BUF_CALL_FLAGS 0x02U     //!< 是CALL
typedef struct msg_buf
{
    void *msg;           //!< buf，长度 @see THREAD_MSG_BUG_LEN
    uhmword_t len;       //!< 这里不是buf的大小，而是存储接收或者发送的长度
    uhmword_t flags;     //!< 传输标志
    thread_t *call_send; //!< 当是call操作时，存放call的发起方
} msg_buf_t;

#define THREAD_MAIGC 0xdeadead
typedef struct thread
{
    kobject_t kobj;           //!< 内核对象节点
    slist_head_t wait;        //!< 用于等待队列
    sched_t sche;             //!< 调度节点
    kobject_t *task;          //!< 绑定的task
    sp_info_t sp;             //!< sp信息
    ram_limit_t *lim;         //!< 内存限制
    ref_counter_t ref;        //!< 引用计数
    msg_buf_t msg;            //!< 每个线程独有的消息缓存区
    enum thread_state status; //!< 线程状态
    umword_t magic;           //!< maigc
} thread_t;

static inline void thread_set_msg_bug(thread_t *th, void *msg, uint16_t len)
{
    th->msg.msg = msg;
    th->msg.len = len;
    th->msg.flags = 0;
}

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
    thread_t *th = (thread_t *)(ALIGN_DOWN(sp, THREAD_BLOCK_SIZE));

    return th;
}

static inline pf_t *thread_get_current_pf(void)
{
    return thread_get_pf(thread_get_current());
}
void thread_init(thread_t *th, ram_limit_t *lim);
void thread_set_exc_regs(thread_t *th, umword_t pc, umword_t user_sp, umword_t ram);
thread_t *thread_create(ram_limit_t *ram);
void thread_bind(thread_t *th, kobject_t *tk);
void thread_unbind(thread_t *th);

void thread_sched(void);
void thread_suspend(thread_t *th);
void thread_ready(thread_t *th, bool_t is_sche);