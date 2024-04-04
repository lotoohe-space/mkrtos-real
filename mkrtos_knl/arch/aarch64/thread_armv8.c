

#include <types.h>
#include <task.h>
#include <thread.h>
#include <esr.h>
#include <syscall.h>
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

    switch (ec)
    {
    case 0x20:
    {
        // mword_t addr = thread_get_pfa();

        // if (vma_page_fault(&tk->vmam, &tk->mem_dir, MASK_LSB(addr, PAGE_SHIFT)) == NULL) {
        //     //TODO: 如果是init进程则内核死机，否则干掉进程
        //     MKRTOS_ASSERT(0);
        // }
    }
        return;
    case 0x24:
    {
        // mword_t addr = thread_get_pfa();

        // if (vma_page_fault(&tk->vmam, &tk->mem_dir, MASK_LSB(addr, PAGE_SHIFT)) == NULL) {
        //     //TODO: 如果是init进程则内核死机，否则干掉进程
        //     MKRTOS_ASSERT(0);
        // }
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
