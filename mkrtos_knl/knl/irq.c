/**
 * @file irq.c
 * @author ATShining (1358745329@qq.com)
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
#include <printk.h>
#include <spinlock.h>

static spinlock_t lock;
static irq_entry_t irqs[CONFIG_IRQ_REG_TAB_SIZE] = {0};
static void irq_tigger(irq_entry_t *irq);

/**
 * @brief 检查指定的irq号是否可用
 *
 * @param inx
 * @return bool_t
 */
bool_t irq_check_usability(int inx)
{
    if (inx >= CONFIG_IRQ_REG_TAB_SIZE)
    {
        return FALSE;
    }
    return irqs[inx].irq_tigger_func == NULL;
}
/**
 * @brief 分配一个可用的irq
 *
 * @param inx
 * @param irq
 * @param irq_tigger_func
 * @return bool_t
 */
bool_t irq_alloc(int inx, void *irq, void (*irq_tigger_func)(irq_entry_t *irq))
{
    umword_t status = spinlock_lock(&lock);

    if (irqs[inx].irq_tigger_func != NULL)
    {
        spinlock_set(&lock, status);
        return FALSE;
    }
    irqs[inx].irq = irq;
    irqs[inx].inx = inx;
    irqs[inx].irq_tigger_func = irq_tigger_func;
    spinlock_set(&lock, status);
    return TRUE;
}
/**
 * @brief 释放一个irq
 *
 * @param inx
 */
void irq_free(int inx)
{
    assert(inx < CONFIG_IRQ_REG_TAB_SIZE);
    irqs[inx].irq_tigger_func = NULL;
}
/**
 * @brief 获取一个irq
 *
 * @param inx
 * @return irq_entry_t*
 */
irq_entry_t *irq_get(int inx)
{
    assert(inx < CONFIG_IRQ_REG_TAB_SIZE);
    return &irqs[inx];
}
/**
 * @brief 中断的入口函数
 *
 */
void entry_handler(void)
{
    umword_t isr_no = arch_get_isr_no();

    if (isr_no < 0)
    {
        return;
    }
    isr_no -= CONFIG_USER_ISR_START_NO; //!< 系统用的irq偏移

    if (isr_no >= CONFIG_IRQ_REG_TAB_SIZE)
    {
        assert(isr_no < CONFIG_IRQ_REG_TAB_SIZE);
    }
    if (!irq_check_usability(isr_no))
    {
        if (irqs[isr_no].irq_tigger_func)
        {
            irqs[isr_no].irq_tigger_func(&irqs[isr_no]);
        }
    }
    else
    {
        arch_disable_irq(isr_no);
    }
}
