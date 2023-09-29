/**
 * @file irq_sender.c
 * @author zhangzheng (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-09-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <arch.h>
#include <types.h>
#include <kobject.h>
#include <thread.h>
#include <ref.h>
#include <init.h>
#include <mm_wrap.h>
#include <factory.h>
#include <irq.h>
#include <task.h>
/**
 * @brief irq sender的操作号
 *
 */
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
    arch_disable_irq(irq->irq->irq_id); //!< 触发中断时关闭中断
    if (irq->irq->wait_thread &&
        thread_get_status(irq->irq->wait_thread) == THREAD_SUSPEND) //!< 线程在休眠时才能唤醒
    {
        thread_ready(irq->irq->wait_thread, TRUE);
    }
    else
    {
        irq->irq->irq_cn++; //!< 否则中断计数+1
    }
}
/**
 * @brief 等待一个中断的来临
 *
 * @param irq
 * @param th
 * @return int
 */
int irq_sender_wait(irq_sender_t *irq, thread_t *th)
{
    umword_t status = cpulock_lock();

    if (!irq->wait_thread)
    {
        irq->wait_thread = th;
        if (irq->irq_cn > 0)
        {
            irq->irq_cn = 0;
            cpulock_set(status);
        }
        else
        {
            thread_suspend(irq->wait_thread);
            cpulock_set(status);
            irq->wait_thread = NULL;
        }
        irq->irq_cn = 0;
        return 0;
    }
    else
    {
        cpulock_set(status);

        return -EACCES;
    }
}
static bool_t irq_sender_unbind(irq_sender_t *irq, int irq_no)
{
    assert(irq);
    if (!irq_check_usability(irq_no) &&
        irq_get(irq_no)->irq == irq) //!< 是否能够解绑检查
    {
        irq_free(irq_no);
        ref_counter_dec(&irq->ref);
        return TRUE;
    }
    else
    {
        return FALSE;
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

        if (irq_alloc(irq_no, irq, irq_tigger) == FALSE)
        {
            //!< 分配失败则返回错误
            tag = msg_tag_init4(0, 0, 0, -ENOENT);
            break;
        }
        irq->irq_id = irq_no;                                              //!< 设置绑定后的irq号
        ref_counter_inc(&irq->ref);                                        //!< 绑定后引用计数+1
        arch_set_enable_irq_prio(irq_no, f->r[2] & 0xffff, f->r[2] >> 16); //!< 绑定时设置优先级
        tag = msg_tag_init4(0, 0, 0, 0);
    }
    break;
    case UNBIND_IRQ:
    {
        umword_t irq_no = f->r[1];
        bool_t suc = irq_sender_unbind(irq, irq_no);

        tag = msg_tag_init4(0, 0, 0, suc ? 0 : -EACCES);
    }
    break;
    case WAIT_IRQ:
    {
        ref_counter_inc(&th->ref);
        int ret = irq_sender_wait(irq, th);
        ref_counter_dec_and_release(&th->ref, &irq->kobj); //! 引用计数+1
        tag = msg_tag_init4(0, 0, 0, ret);
    }
    break;
    case ACK_IRQ:
    {
        arch_enable_irq(irq->irq_id);
        tag = msg_tag_init4(0, 0, 0, 0);
    }
    break;
    default:
        break;
    }

    f->r[0] = tag.raw;
}
static bool_t irq_sender_put(kobject_t *kobj)
{
    irq_sender_t *irq = container_of(kobj, irq_sender_t, kobj);

    return ref_counter_dec(&irq->ref) == 1;
}
static void irq_sender_stage1(kobject_t *kobj)
{
    irq_sender_t *irq = container_of(kobj, irq_sender_t, kobj);
    kobject_invalidate(kobj);            //!< 设置kobj为无效
    irq_sender_unbind(irq, irq->irq_id); //!< 解除绑定
}
static void irq_sender_stage2(kobject_t *kobj)
{
    irq_sender_t *th = container_of(kobj, irq_sender_t, kobj);
    task_t *cur_task = thread_get_current_task();

    mm_limit_free(cur_task->lim, th);
    printk("irq_sender release 0x%x\n", kobj);
}
/**
 * @brief 初始化
 *
 * @param irq
 */
void irq_sender_init(irq_sender_t *irq)
{
    kobject_init(&irq->kobj);
    ref_counter_init(&irq->ref);
    ref_counter_inc(&irq->ref);
    irq->kobj.invoke_func = irq_sender_syscall;
    irq->kobj.stage_1_func = irq_sender_stage1;
    irq->kobj.stage_2_func = irq_sender_stage2;
    irq->kobj.put_func = irq_sender_put;
    irq->irq_id = IRQ_INVALID_NO;
    irq->irq_cn = 0;
}
/**
 * @brief 创建一个irq_sender_t
 *
 * @param lim
 * @return irq_sender_t*
 */
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
/**
 * @brief 创建函数
 *
 * @param lim
 * @param arg0
 * @param arg1
 * @param arg2
 * @param arg3
 * @return kobject_t*
 */
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
