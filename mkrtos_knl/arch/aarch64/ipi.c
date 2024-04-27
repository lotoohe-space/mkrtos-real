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
    assert(msg);
    assert(msg->cb);
    assert(cpulock_get_status());
    assert(!(cpu_mask & (1 << arch_get_current_cpu_id()))); //!< 不能给自己发ipi，否则可能死锁
    assert(!slist_in_list(&msg->node));
    atomic_set(&msg->flags, 0);

    for (int i = 0; i < CONFIG_CPU; i++)
    {
        if (!(cpu_mask & (1 << i)))
        {
            continue;
        }
        ipi_msg_head_t *head = pre_cpu_get_var_cpu(i, &ipi_msg_head);
        umword_t status = spinlock_lock(&head->lock);

        slist_add_append(&head->msg_queue, &msg->node);

        gic2_set_sgir(arm_gicv2_get_global(), (1 << i)); //!< 发送软中断
        spinlock_set(&head->lock, status);
        // preemption();
        // thread_sched(TRUE);
        if (act == IPI_CALL)
        {
            do
            {
                // if (&thread_get_current()->ipi_msg_node != msg)
                {
                    thread_sched(TRUE);
                    // arch_to_sche();
                    // preemption();
                }
            } while (atomic_read(&msg->flags) == 0);
        }
    }
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
        ipi_msg_t *next = slist_next_entry(pos, &head->msg_queue, node);
        slist_del(&pos->node);

        assert(pos->cb);
        pos->ret = pos->cb(pos);
        atomic_set(&pos->flags, 1);
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
