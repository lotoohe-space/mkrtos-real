#pragma once

#include <types.h>
#include <slist.h>
#include <atomics.h>

typedef enum ipi_action
{
    IPI_SEND, //!< 发送消息
    IPI_CALL, //!< 调用
} ipi_action_t;

struct ipi_msg;
typedef struct ipi_msg ipi_msg_t;
typedef int (*ipi_cb_func)(ipi_msg_t *head, bool_t *is_sched);

typedef struct ipi_msg
{
    slist_head_t node; //!< 消息在内存中的节点
    mword_t ret;       //!< 返回值
    umword_t msg;      //!< 第一个消息
    umword_t msg2;     //!< 第二个消息
    umword_t msg3;
    ipi_cb_func cb;      //!< ipi调用的回调函数，这个函数在目标cpu中执行
    void *call_th;       //!< call线程
    ipi_action_t act;    //!< call or send
    umword_t th_time_cn; //!< 时间戳，用于判断线程是否完成切换
    atomic64_t flags;    //!< 用于判断目标cpu是否执行完成
} ipi_msg_t;

void cpu_ipi_to(uint8_t cpu_mask);
void cpu_ipi_init(void);
void cpu_ipi_to_msg(uint8_t cpu_mask, ipi_msg_t *msg, ipi_action_t act);
void cpu_ipi_check(void);
