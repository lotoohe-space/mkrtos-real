
#include <arch.h>
#include <types.h>
#include <kobject.h>
#include <thread.h>
#include <ref.h>
#include <init.h>
#include <mm_wrap.h>
#include <factory.h>
#include <irq.h>

enum irq_sender_op
{
    BIND_IRQ,   //!< 绑定一个中断号
    UNBIND_IRQ, //!< 解除绑定
    WAIT_IRQ,   //!< 等待中断触发
    ACK_IRQ,    //!< 中断确认
};
/**
 * @brief irq_sender的默认中断触发函数
 *
 * @param irq
 */
static void irq_tigger(irq_entry_t *irq)
{
    if (irq->irq->wait_thread && thread_get_status(irq->irq->wait_thread) == THREAD_SUSPEND)
    {
        arch_disable_irq(irq->irq->irq_id);
        thread_ready(irq->irq->wait_thread, TRUE);
    }
}
int irq_sender_wait(irq_sender_t *irq, thread_t *th)
{
    // TODO:临界保护
    if (!irq->wait_thread)
    {
        irq->wait_thread = th;
        ref_counter_inc(&irq->wait_thread->ref); //! 线程引用计数+1
        thread_suspend(irq->wait_thread);
        ref_counter_dec(&irq->wait_thread->ref); //! 线程引用计数+1
        irq->wait_thread = NULL;
        return 0;
    }
    else
    {
        return -EACCES;
    }
}
void irq_sender_syscall(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f)
{
    assert(kobj);
    assert(f);
    msg_tag_t tag = msg_tag_init4(0, 0, 0, -EINVAL);
    thread_t *th = thread_get_current();
    irq_sender_t *irq = container_of(kobj, irq_sender_t, kobj);

    if (sys_p.prot != IRQ_PROT)
    {
        f->r[0] = msg_tag_init4(0, 0, 0, -EPROTO).raw;
        return;
    }
    switch (sys_p.op)
    {
    case BIND_IRQ:
    {
        umword_t irq_no = f->r[1];
        if (irq_check_usability(irq_no))
        {
            irq->irq_id = irq_no;
            irq_alloc(irq_no, irq, irq_tigger);
            ref_counter_inc(&irq->ref);
            tag = msg_tag_init4(0, 0, 0, 0);
        }
        else
        {
            tag = msg_tag_init4(0, 0, 0, -EACCES);
        }
    }
    break;
    case UNBIND_IRQ:
    {
        umword_t irq_no = f->r[1];
        if (!irq_check_usability(irq_no) && irq_get(irq_no)->irq == irq)
        {
            irq_free(irq_no);
            ref_counter_dec(&irq->ref);
            tag = msg_tag_init4(0, 0, 0, 0);
        }
        else
        {
            tag = msg_tag_init4(0, 0, 0, -EACCES);
        }
    }
    break;
    case WAIT_IRQ:
    {
        int ret = irq_sender_wait(irq, th);
        msg_tag_init4(0, 0, 0, ret);
    }
    break;
    case ACK_IRQ:
    {
        arch_enable_irq(irq->irq_id);
    }
    break;
    default:
        break;
    }
    f->r[0] = tag.raw;
}
void irq_sender_init(irq_sender_t *irq)
{
    kobject_init(&irq->kobj);
    ref_counter_init(&irq->ref);
    ref_counter_inc(&irq->ref);
    irq->kobj.invoke_func = irq_sender_syscall;
    irq->irq_id = IRQ_INVALID_NO;
}
static irq_sender_t *irq_create(ram_limit_t *lim)
{
    irq_sender_t *irq = mm_limit_alloc(lim, sizeof(irq_sender_t));

    if (!irq)
    {
        return NULL;
    }
    irq_sender_init(irq);
    return irq;
}
static kobject_t *irq_create_func(ram_limit_t *lim, umword_t arg0, umword_t arg1,
                                  umword_t arg2, umword_t arg3)
{
    irq_sender_t *irq = irq_create(lim);

    if (!irq)
    {
        return NULL;
    }
    return &irq->kobj;
}

/**
 * @brief 工厂注册函数
 *
 */
static void irq_factory_register(void)
{
    factory_register(irq_create_func, IRQ_PROT);
}
INIT_KOBJ(irq_factory_register);
