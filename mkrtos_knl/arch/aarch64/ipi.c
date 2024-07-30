#include <arch.h>
#include <init.h>
#include <arm_gicv2.h>
#include <ipi.h>
#include <irq.h>
#include <slist.h>
#include <pre_cpu.h>
#include <spinlock.h>
#include <assert.h>
#include <atomics.h>
#include <timer.h>
// ipi call流程
// 当前线程发送ipi call后，进入挂起状态
// 收到消息的核处理完成消息后，给发送的核发送指定线程唤醒的消息（send）方式发送。

struct ipi_msg_head;
typedef struct ipi_msg_head ipi_msg_head_t;

typedef struct ipi_msg_head
{
    slist_head_t msg_queue;
    spinlock_t lock;
} ipi_msg_head_t;

static PER_CPU(ipi_msg_head_t, ipi_msg_head);

void cpu_ipi_to(uint8_t cpu_mask)
{
    mword_t status = spinlock_lock(&arm_gicv2_get_global()->lock);

    gic2_set_sgir(arm_gicv2_get_global(), cpu_mask);
    spinlock_set(&arm_gicv2_get_global()->lock, status);
}
void cpu_ipi_to_msg(uint8_t cpu_mask, ipi_msg_t *msg, ipi_action_t act)
{
    thread_t *cur_th = thread_get_current();
    assert(msg);
    assert(msg->cb);
    assert(cpulock_get_status());
    assert(!(cpu_mask & (1 << arch_get_current_cpu_id()))); //!< 不能给自己发ipi，否则可能死锁
    assert(!slist_in_list(&msg->node));

    for (int i = 0; i < CONFIG_CPU; i++)
    {
        if (!(cpu_mask & (1 << i)))
        {
            continue;
        }
        ipi_msg_head_t *head = pre_cpu_get_var_cpu(i, &ipi_msg_head);
        umword_t cpu_lock_status = cpulock_lock();
        umword_t status = spinlock_lock(&head->lock);

        slist_add_append(&head->msg_queue, &msg->node);

        msg->call_th = cur_th;
        msg->act = act;
        spinlock_set(&head->lock, status);
        gic2_set_sgir(arm_gicv2_get_global(), (1 << i)); //!< 发送软中断
        if (act == IPI_CALL)
        {
            msg->th_time_cn = atomic_read(&cur_th->time_count);
            thread_suspend_sw(cur_th, FALSE); //!< 这里必须等待切换后才能发送中断，不然切回来的时候会挂TODO:
            preemption();
        }
        cpulock_set(cpu_lock_status);
    }
}
static int thread_ipi_call_reply(ipi_msg_t *msg, bool_t *is_sched)
{
    thread_t *send_th = container_of(msg, thread_t, ipi_msg_node);

    thread_ready(send_th, TRUE);
    return 0;
}
static void hard_delay(void)
{
    volatile int i = 5000000;
    while (i--);
}
/**
 * 两个核相互发送消息可能会死锁
 */
void cpu_ipi_handler(irq_entry_t *entry)
{

    int cur_cpu = arch_get_current_cpu_id();

    ipi_msg_head_t *head = pre_cpu_get_var_cpu(cur_cpu, &ipi_msg_head);

    umword_t status = spinlock_lock(&head->lock);
    ipi_msg_t *pos;

    slist_foreach_not_next(pos, &head->msg_queue, node)
    {
        bool_t is_sched = TRUE;
        ipi_msg_t *next = slist_next_entry(pos, &head->msg_queue, node);
        thread_t *call_th = (thread_t *)(pos->call_th);
        slist_del(&pos->node);

        assert(pos->cb);
        // while (atomic_read(&call_th->time_count) == pos->th_time_cn) {
        //     preemption();
        // }
        // 等待切换完成TODO:
        if (pos->msg == (umword_t)call_th) {
            hard_delay();
        }
        pos->ret = pos->cb(pos, &is_sched);
        if (pos->act == IPI_CALL)
        {
            if (is_sched)
            {
                // CALL需要回复消息
                call_th->ipi_msg_node.cb = thread_ipi_call_reply;
                cpu_ipi_to_msg((1 << call_th->cpu),
                               &call_th->ipi_msg_node, IPI_SEND);
            }
        }
        pos = next;
    }

    spinlock_set(&head->lock, status);
    gic2_eoi_irq(arm_gicv2_get_global(), entry->inx);
}

void cpu_ipi_init(void)
{
    ipi_msg_head_t *head = pre_cpu_get_current_cpu_var(&ipi_msg_head);

    slist_init(&head->msg_queue);
    spinlock_init(&head->lock);
    irq_alloc(0, NULL, cpu_ipi_handler);
    gic2_set_prio(arm_gicv2_get_global(), 0, 1);
    // FIXME：目前IPI的中断优先级必须比tiemr的低，不然可能会死锁。
}
