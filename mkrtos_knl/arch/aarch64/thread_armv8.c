

#include <types.h>
#include <task.h>
#include <thread.h>
#include <esr.h>
#include <syscall.h>
#include <vma.h>
#include <thread_knl.h>
umword_t thread_get_pfa(void)
{
    umword_t a;

    asm volatile("mrs %0, far_el2" : "=r"(a));
    return a;
}
void thread_sync_entry(entry_frame_t *regs)
{
    umword_t ec = arm_esr_ec(esr_get());
    thread_t *th = thread_get_current();
    task_t *tk = thread_get_bind_task(th);

    switch (ec)
    {
    case 0x20:
    {
        mword_t addr = thread_get_pfa();

        if (task_vma_page_fault(&tk->mm_space.mem_vma, ALIGN_DOWN(addr, PAGE_SIZE)) < 0)
        {
            printk("0x20 pfa:0x%x\n", addr);
            task_knl_kill(th, FALSE);
        }
    }
        return;
    case 0x24:
    {
        mword_t addr = thread_get_pfa();

        if (task_vma_page_fault(&tk->mm_space.mem_vma, ALIGN_DOWN(addr, PAGE_SIZE)) < 0)
        {
            printk("0x24 pfa:0x%x\n", addr);
            task_knl_kill(th, FALSE);
        }
    }
        return;
    case 0x12:
    case 0x11:
    case 0x15:
    case 0x16:
        syscall_entry(regs);
        return;
    case 0x00:
        return;
    case 0x07:
        break;
    case 0x03:
    default:
        break;
    }
}
