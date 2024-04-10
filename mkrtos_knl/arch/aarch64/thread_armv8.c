

#include <esr.h>
#include <syscall.h>
#include <task.h>
#include <thread.h>
#include <thread_knl.h>
#include <types.h>
#include <vma.h>
umword_t thread_get_pfa(void)
{
    umword_t a;

    asm volatile("mrs %0, far_el2" : "=r"(a));
    return a;
}
static void dump_stack(umword_t pc, umword_t x29)
{
    task_t *th = thread_get_current_task();
    uint64_t result[5];

    printk("pc:0x%x x29:0x%x\n", pc, x29);
    for (size_t i = 0; i < sizeof(result) / sizeof(umword_t); i++) {
        umword_t knl_addr;
        knl_addr = (umword_t)mm_get_paddr(mm_space_get_pdir(&th->mm_space), ALIGN_DOWN(x29, PAGE_SIZE), PAGE_SHIFT) + (x29 - ALIGN_DOWN(x29, PAGE_SIZE));
        if (knl_addr) {
            result[i] = (*(umword_t *)(knl_addr + 8)) - 4;
            x29 = *(umword_t *)(knl_addr);
        }
    }
    printk("knl pf stack: 0x%x,0x%x,0x%x,0x%x,0x%x\n", result[0], result[1], result[2], result[3], result[4]);
}
void thread_sync_entry(entry_frame_t *regs)
{
    umword_t ec = arm_esr_ec(esr_get());
    thread_t *th = thread_get_current();
    task_t *tk = thread_get_bind_task(th);

    switch (ec) {
    case 0x20: {
        mword_t addr = thread_get_pfa();

        if (task_vma_page_fault(&tk->mm_space.mem_vma, ALIGN_DOWN(addr, PAGE_SIZE)) < 0) {
            printk("[knl]0x20 pfa:0x%x\n", addr);
            dump_stack(regs->pc, regs->regs[29]);
            task_knl_kill(th, FALSE);
        }
    }
        return;
    case 0x24: {
        mword_t addr = thread_get_pfa();

        if (task_vma_page_fault(&tk->mm_space.mem_vma, ALIGN_DOWN(addr, PAGE_SIZE)) < 0) {
            printk("[knl]0x24 pfa:0x%x\n", addr);
            dump_stack(regs->pc, regs->regs[29]);
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
