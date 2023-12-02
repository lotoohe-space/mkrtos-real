/**
 * @file thread.h
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-09-14
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include "kobject.h"
#include "scheduler.h"
#include "util.h"
#include "arch.h"
#include "ref.h"
#define THREAD_BLOCK_SIZE 0x400 //!< 线程块大小，栈在块的顶部
struct task;
typedef struct task task_t;
struct thread;
typedef struct thread thread_t;

#define THREAD_MSG_BUG_LEN 128UL     //!< 最小的消息寄存器大小
#define MSG_BUF_HAS_DATA_FLAGS 0x01U //!< 已经有数据了
#define MSG_BUF_RECV_R_FLAGS 0x02U   //!< 接收来自recv_th的消息
#define MSG_BUF_REPLY_FLAGS 0x04U    //!< 回复消息给send_th

#define IPC_MSG_SIZE 96
#define MAP_BUF_SIZE 16
#define IPC_USER_SIZE 12

#if (IPC_MSG_SIZE + MAP_BUF_SIZE + IPC_USER_SIZE) > THREAD_MSG_BUG_LEN
#error "IPC MSG len error."
#endif

typedef struct ipc_msg
{
    union
    {
        struct
        {
            umword_t msg_buf[IPC_MSG_SIZE / WORD_BYTES];
            umword_t map_buf[MAP_BUF_SIZE / WORD_BYTES];
            umword_t user[IPC_USER_SIZE / WORD_BYTES];
        };
        uint8_t data[THREAD_MSG_BUG_LEN];
    };
} ipc_msg_t;

typedef union ipc_timeout
{
    umword_t raw;
    struct
    {
        uhmword_t send_timeout;
        uhmword_t recv_timeout;
    };
} ipc_timeout_t;

static inline ipc_timeout_t ipc_timeout_create2(uhmword_t send_timeout, uhmword_t recv_timeout)
{
    return (ipc_timeout_t){
        .send_timeout = send_timeout,
        .recv_timeout = recv_timeout,
    };
}
static inline ipc_timeout_t ipc_timeout_create(umword_t raw)
{
    return (ipc_timeout_t){
        .raw = raw,
    };
}

enum thread_state
{
    THREAD_IDLE = 0,    //!< 空闲状态
    THREAD_DEAD = 1,    //!< 死亡状态
    THREAD_SUSPEND = 2, //!< 只有接收和发送ipc消息时才能挂起
    THREAD_READY = 3,   //!< 在就绪队列中
    THREAD_TODEAD = 4,  //!< 该标志标志线程马上要死亡了，执行完必要操作后，进入THREAD_DEAD状态
};
enum thread_ipc_state
{
    THREAD_NONE,
    THREAD_SEND,
    THREAD_RECV,
    THREAD_WAIT,
    THREAD_TIMEOUT,
    THREAD_IPC_ABORT,
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

typedef struct msg_buf
{
    void *msg;     //!< buf，长度是固定的 @see THREAD_MSG_BUG_LEN
    msg_tag_t tag; //!< 存放发送的临时标识
} msg_buf_t;

#define THREAD_MAGIC 0xdeadead //!< 用于栈溢出检测
typedef struct thread
{
    kobject_t kobj;    //!< 内核对象节点
    sched_t sche;      //!< 调度节点
    kobject_t *task;   //!< 绑定的task
    sp_info_t sp;      //!< sp信息
    ram_limit_t *lim;  //!< 内存限制
    ref_counter_t ref; //!< 引用计数

    slist_head_t futex_node; //!< futex使用

    msg_buf_t msg;               //!< 每个线程独有的消息缓存区
    slist_head_t wait_send_head; //!< 等待头，那些节点等待给当前线程发送数据
    thread_t *last_send_th;      //!< 当前线程上次接收到谁的数据
    umword_t user_id;            //!< 接收到的user_id

    enum thread_state status;         //!< 线程状态
    enum thread_ipc_state ipc_status; //!< ipc状态

    umword_t magic; //!< maigc
} thread_t;

static inline void thread_set_msg_bug(thread_t *th, void *msg)
{
    th->msg.msg = msg;
}
static inline void *thread_get_msg_buf(thread_t *th)
{
    return th->msg.msg;
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
task_t *thread_get_current_task(void);
task_t *thread_get_bind_task(thread_t *th);

static inline pf_t *thread_get_current_pf(void)
{
    return thread_get_pf(thread_get_current());
}
void thread_init(thread_t *th, ram_limit_t *lim);
void thread_set_exc_regs(thread_t *th, umword_t pc, umword_t user_sp, umword_t ram, umword_t stack);
thread_t *thread_create(ram_limit_t *ram);
void thread_bind(thread_t *th, kobject_t *tk);
void thread_unbind(thread_t *th);

void thread_send_wait(thread_t *th);
void thread_sched(void);
void thread_suspend(thread_t *th);
void thread_dead(thread_t *th);
void thread_todead(thread_t *th, bool_t is_sche);
void thread_ready(thread_t *th, bool_t is_sche);

void thread_timeout_check(ssize_t tick);
msg_tag_t thread_do_ipc(kobject_t *kobj, entry_frame_t *f, umword_t user_id);
