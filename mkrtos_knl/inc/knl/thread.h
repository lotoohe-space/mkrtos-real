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
#include <atomics.h>
struct task;
typedef struct task task_t;
struct thread;
typedef struct thread thread_t;

#define THREAD_IS_DEBUG 0 //!< 是否开启调试

#define THREAD_CREATE_VM 0x1

#define THREAD_MSG_BUG_LEN CONFIG_THREAD_MSG_BUG_LEN //!< 最小的消息寄存器大小
#define MSG_BUF_HAS_DATA_FLAGS 0x01U                 //!< 已经有数据了
#define MSG_BUF_RECV_R_FLAGS 0x02U                   //!< 接收来自recv_th的消息
#define MSG_BUF_REPLY_FLAGS 0x04U                    //!< 回复消息给send_th

#define IPC_MSG_SIZE (CONFIG_THREAD_IPC_MSG_LEN * sizeof(void*))
#define MAP_BUF_SIZE (CONFIG_THREAD_MAP_BUF_LEN * sizeof(void*))
#define IPC_USER_SIZE (CONFIG_THREAD_USER_BUF_LEN * sizeof(void*))

#if IS_ENABLED(CONFIG_VCPU)
#define IPC_VPUC_MSG_OFFSET (3 * 1024) //!< vcpu 传递消息的偏移量
#endif

// #if ((IPC_MSG_SIZE * WORD_BYTES) + (MAP_BUF_SIZE * WORD_BYTES) + (IPC_USER_SIZE * WORD_BYTES)) > THREAD_MSG_BUG_LEN
// #error "IPC MSG len error."
// #endif

typedef struct ipc_msg
{
    union
    {
        struct
        {
            umword_t msg_buf[CONFIG_THREAD_IPC_MSG_LEN];
            umword_t map_buf[CONFIG_THREAD_MAP_BUF_LEN];
            umword_t user[CONFIG_THREAD_USER_BUF_LEN]; //!< 0:pthread使用 1:存放私有信息 2:源端的PID号 3:存放私有信息
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
    THREAD_NONE = 0,
    THREAD_SEND = 1,
    THREAD_RECV = 2,
    THREAD_WAIT = 4,
    THREAD_TIMEOUT = 8,
    THREAD_IPC_ABORT = 16,
};

typedef struct msg_buf
{
    void *msg; //!< buf，长度是固定的 @see THREAD_MSG_BUG_LEN
#if IS_ENABLED(CONFIG_MMU)
    void *umsg; //!< 消息对应的内核的地址
#endif
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

#if IS_ENABLED(CONFIG_SMP)
    ipi_msg_t ipi_msg_node;
#endif
    int cpu;
    atomic_t time_count; //!< 运行的时间
    bool_t is_vcpu;      //!< 是否是vcpu

    msg_buf_t msg; //!< 每个线程独有的消息缓存区

    slist_head_t wait_send_head; //!< 等待头，那些节点等待给当前线程发送数据
    spinlock_t recv_lock;        //!< 当前线程接收消息时锁住
    spinlock_t send_lock;        //!< 当前线程发送消息时锁住
    bool_t has_wait_send_th;    //!< 有线程等待给当前线程发送消息

    thread_t *last_send_th; //!< 当前线程上次接收到谁的数据
    kobject_t *ipc_kobj;    //!< 发送者放到一个ipc对象中
    umword_t user_id;       //!< 接收到的user_id

    enum thread_state status;         //!< 线程状态
    enum thread_ipc_state ipc_status; //!< ipc状态

    umword_t magic; //!< maigc
} thread_t;

static inline int thread_get_cpu(thread_t *th)
{
    return th->cpu;
}
static inline void thread_set_cpu(thread_t *th, int cpu)
{
    th->cpu = cpu;
    _dmb(ish);
}
static inline enum thread_state thread_get_status(thread_t *th)
{
    return th->status;
}
static inline void thread_set_state(thread_t *th, enum thread_state status)
{
    th->status = status;
    _dmb(ish);
}
static inline void thread_set_ipc_state(thread_t *th, enum thread_ipc_state ipc_status)
{
    th->ipc_status = ipc_status;
    _dmb(ish);
}
static inline enum thread_ipc_state thread_get_ipc_state(thread_t *th)
{
    return th->ipc_status;
}
static inline void thread_set_msg_buf(thread_t *th, void *msg, void *umsg)
{
    th->msg.msg = msg;
#if IS_ENABLED(CONFIG_MMU)
    th->msg.umsg = umsg;
#endif
}
static inline void *thread_get_msg_buf(thread_t *th)
{
#if IS_ENABLED(CONFIG_MMU)
    return th->msg.umsg;
#else
    return th->msg.msg;
#endif
}
static inline void *thread_get_kmsg_buf(thread_t *th)
{
    return th->msg.msg;
}

static inline pf_t *thread_get_pf(thread_t *th)
{
    uint8_t *bottom = (uint8_t *)th;

    return ((pf_t *)(bottom + CONFIG_THREAD_BLOCK_SIZE)) - 1;
}

static inline thread_t *thread_get_current(void)
{
    umword_t sp = arch_get_sp();
    thread_t *th = (thread_t *)(ALIGN_DOWN(sp, CONFIG_THREAD_BLOCK_SIZE));

    return th;
}
task_t *thread_get_current_task(void);
task_t *thread_get_task(thread_t *th);
task_t *thread_get_bind_task(thread_t *th);

static inline pf_t *thread_get_current_pf(void)
{
    return thread_get_pf(thread_get_current());
}
void thread_init(thread_t *th, ram_limit_t *lim, umword_t flags);
void thread_set_exc_regs(thread_t *th, umword_t pc, umword_t user_sp, umword_t ram, umword_t stack);
thread_t *thread_create(ram_limit_t *ram, umword_t flags);
void thread_bind(thread_t *th, kobject_t *tk);
void thread_unbind(thread_t *th);

void thread_send_wait(thread_t *th);
bool_t thread_sched(bool_t is_sche);
int thread_suspend_remote(thread_t *th, bool_t is_sche);
void thread_suspend(thread_t *th);
void thread_dead(thread_t *th);
void thread_todead(thread_t *th, bool_t is_sche);
void thread_ready(thread_t *th, bool_t is_sche);
void thread_ready_remote(thread_t *th, bool_t is_sche);
void thread_suspend_sw(thread_t *th, bool_t is_sche);

void thread_timeout_check(ssize_t tick);
msg_tag_t thread_do_ipc(kobject_t *kobj, entry_frame_t *f, umword_t user_id);
int thread_ipc_call(thread_t *to_th, msg_tag_t in_tag, msg_tag_t *ret_tag,
                    ipc_timeout_t timout, umword_t *ret_user_id, bool_t is_call);
