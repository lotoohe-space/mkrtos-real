#pragma once
#include "types.h"
#include "ref.h"
struct ipc;
typedef struct ipc ipc_t;
struct thread;
typedef struct thread thread_t;
/**
 * @brief ipc 对象，用于夸进程消息发送
 *
 */
typedef struct ipc
{
    kobject_t kobj;         //!< 内核对象
    spinlock_t lock;        //!< 操作的锁
    thread_t *svr_th;       //!< 绑定的服务端线程
    slist_head_t wait_bind; //!<
    ram_limit_t *lim;       //!< 内存限额
    umword_t user_id;       //!< 服务端绑定的数据
    ref_counter_t ref;      //!< 引用计数
} ipc_t;

int ipc_bind(ipc_t *ipc, obj_handler_t th_hd, umword_t user_id, thread_t *th_kobj);
