

#include <esr.h>
#include <syscall.h>
#include <task.h>
#include <thread.h>
#include <thread_knl.h>
#include <types.h>
#include <vma.h>
#include "thread_armv8.h"
umword_t thread_get_pfa(void)
{
    thread_t *th = thread_get_current();
    umword_t a;
    umword_t far_el2;
    asm volatile("mrs %0, far_el2" : "=r"(far_el2));

    if (th->is_vcpu)
    {
        asm volatile("mrs %0, hpfar_el2" : "=r"(a));
        a >>= 4;
        a <<= CONFIG_PAGE_SHIFT;

        a |= far_el2 & ((1 << CONFIG_PAGE_SHIFT) - 1);
    }
    else
    {
        a = far_el2;
    }
    return a;
}
static void dump_stack(umword_t pc, umword_t x29)
{
    task_t *th = thread_get_current_task();
    uint64_t result[5];

    printk("pc:0x%x x29:0x%x\n", pc, x29);
    for (size_t i = 0; i < sizeof(result) / sizeof(umword_t); i++)
    {
        umword_t knl_addr;
        knl_addr = (umword_t)mm_get_paddr(mm_space_get_pdir(&th->mm_space), ALIGN_DOWN(x29, PAGE_SIZE), PAGE_SHIFT) + (x29 - ALIGN_DOWN(x29, PAGE_SIZE));
        if (knl_addr)
        {
            result[i] = (*(umword_t *)(knl_addr + 8)) - 4;
            x29 = *(umword_t *)(knl_addr);
        }
    }
    printk("knl pf stack: 0x%lx,0x%lx,0x%lx,0x%lx,0x%lx\n", result[0], result[1], result[2], result[3], result[4]);
}
#if IS_ENABLED(CONFIG_VCPU)
static int thread_exec_to_vcpu(thread_t *th, entry_frame_t *regs, umword_t esr, addr_t ipa_addr)
{
    ipc_msg_t *msg;
    msg_tag_t ret_tag;
    umword_t user_id;
    entry_frame_t *dst_pf;
    task_t *tk = thread_get_bind_task(th);

    msg = thread_get_kmsg_buf((thread_t *)(th));
    msg->msg_buf[0] = esr;
    msg->msg_buf[1] = ipa_addr;
    thread_t *to_th = (thread_t *)(tk->exec_th);
    if (to_th == NULL)
    {
        return -1;
    }
    ipc_msg_t *to_msg = thread_get_kmsg_buf(to_th);

    dst_pf = (entry_frame_t *)(to_msg->data + IPC_VPUC_MSG_OFFSET);
    *dst_pf = *regs;
    sti();
    int ret = thread_ipc_call(to_th, msg_tag_init4(0, 2, 0, 0),
                              &ret_tag, ipc_timeout_create2(0, 0),
                              &user_id, TRUE);
    cli();
    if (ret >= 0)
    {
        *regs = *dst_pf;
    }
    else
    {
        return ret;
    }
    return msg_tag_get_val(ret_tag);
}
#endif
void thread_sync_entry(entry_frame_t *regs)
{
    umword_t esr = esr_get();
    uint32_t ec, il, iss;
    thread_t *th = thread_get_current();
    task_t *tk = thread_get_bind_task(th);
    int ret = -1;

    ec = (esr & ESR_EC_MASK) >> ESR_EC_SHIFT;
    il = (esr & ESR_IL_MASK) >> ESR_IL_SHIFT;
    iss = (esr & ESR_ISS_MASK) >> ESR_ISS_SHIFT;
    // printk("user:0x%x ec:0x%x\n", !ec, ec);

    switch (ec)
    {
    case EC_TRAP_LWREL_INST_ABORT:
    {
        mword_t addr = thread_get_pfa();

        switch (iss & ISS_ABORT_FSC_MASK)
        {
        case FSC_TRANS_FAULT_LEVEL1:
        case FSC_TRANS_FAULT_LEVEL2:
        case FSC_TRANS_FAULT_LEVEL3:
            ret = task_vma_page_fault(&tk->mm_space.mem_vma, ALIGN_DOWN(addr, PAGE_SIZE));
            if (ret < 0)
            {
                printk("[knl] inst abort 0x20 pfa:0x%lx\n", addr);
                dump_stack(regs->pc, regs->regs[29]);
                task_knl_kill(th, FALSE);
            }
            break;
        }
    }
        return;
    case EC_TRAP_LWREL_DATA_ABORT:
    {
        mword_t addr = thread_get_pfa();

        switch (iss & ISS_ABORT_FSC_MASK)
        {
        case FSC_TRANS_FAULT_LEVEL1:
        case FSC_TRANS_FAULT_LEVEL2:
        case FSC_TRANS_FAULT_LEVEL3:
            ret = task_vma_page_fault(&tk->mm_space.mem_vma, ALIGN_DOWN(addr, PAGE_SIZE));
            if (ret < 0)
            {
                dump_stack(regs->pc, regs->regs[29]);
                task_knl_kill(th, FALSE);
            }
            break;
        case FSC_ACCESS_FAULT_LEVEL1:
        case FSC_ACCESS_FAULT_LEVEL2:
        case FSC_ACCESS_FAULT_LEVEL3:
            // printk("[knl] data abort 0x20 pfa:0x%lx\n", addr);
            // dump_stack(regs->pc, regs->regs[29]);
#if IS_ENABLED(CONFIG_VCPU)
            ret = thread_exec_to_vcpu(th, regs, esr, addr);
            if (ret < 0)
            {
                dump_stack(regs->pc, regs->regs[29]);
                task_knl_kill(th, FALSE);
            }
            // printk("%s:%d ret:%d\n", __func__, __LINE__, ret);
#else
            dump_stack(regs->pc, regs->regs[29]);
            task_knl_kill(th, FALSE);
#endif
            break;
        }
    }
        return;
    case EC_TRAP_HVC_A32:
    case EC_TRAP_SVC_A32:
    case EC_TRAP_SVC_A64:
    case EC_TRAP_HVC_A64:
        syscall_entry(regs);
        return;
    case EC_UNKNOWN:
        printk(" ec:0x%x\n", ec);
        return;
    case EC_SIMD_FPU:
        printk(" ec:0x%x\n", ec);
        break;
    case EC_TRAP_MCR_MRC_CP15_A32:
        printk(" ec:0x%x\n", ec);
    default:
        printk("unknown ec:0x%x\n", ec);
        break;
    }
}
