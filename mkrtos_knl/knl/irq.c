
#include <arch.h>
#include <types.h>
#include <kobject.h>
#include <thread.h>
#include <ref.h>
#include <init.h>
#include <mm_wrap.h>
#include <factory.h>
#include <irq.h>

/*TODO:换成更节省内存的方式*/
static irq_entry_t irqs[IRQ_REG_TAB_SIZE] = {0};
static void irq_tigger(irq_entry_t *irq);

bool_t irq_check_usability(int inx)
{
    if (inx >= IRQ_REG_TAB_SIZE)
    {
        return FALSE;
    }
    return irqs[inx].irq_tigger_func == NULL;
}
void irq_alloc(int inx, irq_sender_t *irq, void (*irq_tigger_func)(irq_entry_t *irq))
{
    assert(irqs[inx].irq_tigger_func == NULL);
    irqs[inx].irq = irq;
    irqs[inx].irq_tigger_func = irq_tigger_func;
}
void irq_free(int inx)
{
    assert(inx < IRQ_REG_TAB_SIZE);
    irqs[inx].irq_tigger_func = NULL;
}
irq_entry_t *irq_get(int inx)
{
    assert(inx < IRQ_REG_TAB_SIZE);
    return &irqs[inx];
}
/**
 * @brief 中断的入口函数
 *
 */
void entry_handler(void)
{
    umword_t isr_no = arch_get_isr_no();

    isr_no -= USER_ISR_START_NO;

    if (!irq_check_usability(isr_no))
    {
        if (irqs[isr_no].irq_tigger_func)
        {
            irqs[isr_no].irq_tigger_func(&irqs[isr_no]);
        }
    }
}
