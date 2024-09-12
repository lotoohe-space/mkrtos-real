/**
 * @file thread_armv7m.c
 * @author ATShining (135874329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-09-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "types.h"
#include "thread.h"
#include "printk.h"
#include "app.h"
#include "mm_wrap.h"
#include "arch.h"
#include "string.h"
#include <asm/system.h>
#include <mmu.h>
#include <task.h>
#if IS_ENABLED(CONFIG_VCPU)
#include <hyp.h>
#endif
void thread_arch_init(thread_t *th, umword_t flags)
{
    th->sp.hyp.par_el1 = read_sysreg(par_el1);
    th->sp.hyp.cntv_cval_el0 = read_sysreg(cntv_cval_el0);
    th->sp.hyp.cntkctl_el1 = read_sysreg(cntkctl_el1);
    th->sp.hyp.cntv_ctl_el0 = read_sysreg(cntv_ctl_el0);
    th->sp.hyp.tpidr_el1 = 0;
    th->sp.hyp.sp_el1 = 0;
    th->sp.hyp.elr_el1 = 0;
    th->sp.hyp.vbar_el1 = 0;
    th->sp.hyp.cpacr_el1 = read_sysreg(cpacr_el1);
    th->sp.hyp.spsr_fiq = 0;
    th->sp.hyp.spsr_irq = 0;
    th->sp.hyp.spsr_el1 = 0;
    th->sp.hyp.spsr_abt = 0;
    th->sp.hyp.spsr_und = 0;
    th->sp.hyp.csselr_el1 = read_sysreg(csselr_el1);
#if !IS_ENABLED(CONFIG_VCPU)
    th->sp.hyp.hcr_el2 = read_sysreg(hcr_el2);
#else
    if (flags & THREAD_CREATE_VM)
    {
        th->is_vcpu = TRUE;
        //| (1UL << 34)
        // asm volatile("msr HCR_EL2, %x0" : : "r"(Hcr_host_bits | (1UL << 13) | (1UL << 14)));
        // asm volatile("msr HSTR_EL2, %x0" : : "r"(Hstr_vm));
        th->sp.hyp.hcr_el2 = 0x30023f | (1UL << 10) | (3UL << 13) | (1UL << 31); // read_sysreg(hcr_el2);0x30023f|(1UL<<10) |(3UL<<13)
        th->sp.sysregs.hstr_el2 = Hstr_vm;                                       // read_sysreg(hstr_el2);
        th->sp.sysregs.vmpidr_el2 = 0;                                           // TODO:虚拟mpidr_el1寄存器
        th->sp.sysregs.sp_el0 = read_sysreg(sp_el0);
        th->sp.sysregs.sctlr_el1 = read_sysreg(sctlr_el1);
        th->sp.sysregs.ttbr0_el1 = read_sysreg(ttbr0_el1);
        th->sp.sysregs.ttbr1_el1 = read_sysreg(ttbr1_el1);
        th->sp.sysregs.tcr_el1 = read_sysreg(tcr_el1);
        th->sp.sysregs.esr_el1 = read_sysreg(esr_el1);
        th->sp.sysregs.far_el1 = read_sysreg(far_el1);
        th->sp.sysregs.mair_el1 = read_sysreg(mair_el1);
        th->sp.sysregs.contextidr_el1 = read_sysreg(contextidr_el1);
        th->sp.sysregs.tpidrro_el0 = read_sysreg(tpidrro_el0);
    }
    else
    {
        th->sp.hyp.hcr_el2 = read_sysreg(hcr_el2);
    }
#endif
}

extern void ret_form_run(void);
syscall_entry_func syscall_handler_get(void)
{
    return syscall_entry;
}
void thread_knl_pf_set(thread_t *cur_th, void *pc)
{
    pf_t *pt = ((pf_t *)((char *)cur_th + CONFIG_THREAD_BLOCK_SIZE)) - 1;

    pt->pstate = PSR_MODE_EL2h;
    // pt->pc = (umword_t)pc;
    cur_th->sp.x19 = (umword_t)pc;
    cur_th->sp.x20 = 0 /*arg*/;
    cur_th->sp.pc = (mword_t)ret_form_run;
    cur_th->sp.sp = (umword_t)pt;
}
void thread_user_pf_set(thread_t *cur_th, void *pc, void *user_sp, void *ram, umword_t stack)
{
    pf_t *pt = ((pf_t *)((char *)cur_th + CONFIG_THREAD_BLOCK_SIZE)) - 1;

    pt->pstate = PSR_MODE_EL1h;
    pt->pc = (umword_t)pc;
    pt->sp = (umword_t)user_sp;
    cur_th->sp.x19 = 0;
    cur_th->sp.x20 = 0;
    cur_th->sp.pc = (mword_t)ret_form_run;
    cur_th->sp.sp = (umword_t)pt;
    cur_th->sp.hyp.sp_el1 = (umword_t)user_sp;
}
void task_knl_init(task_t *knl_tk)
{
    knl_tk->mm_space.mem_dir = *boot_get_pdir();
}

paddr_t task_get_currnt_paddr(vaddr_t vaddr)
{
    umword_t paddr;
    page_entry_t *pdir = mm_space_get_pdir(&(thread_get_current_task()->mm_space));
    umword_t vaddr_align = ALIGN_DOWN(vaddr, PAGE_SIZE);

    paddr = mm_get_paddr(pdir, vaddr_align, PAGE_SHIFT);
    if (paddr == 0)
    {
        return paddr;
    }
    paddr += vaddr - vaddr_align;
    return paddr;
}
paddr_t task_get_paddr(task_t *task, vaddr_t vaddr)
{
    umword_t paddr;
    page_entry_t *pdir = mm_space_get_pdir(&(task->mm_space));
    umword_t vaddr_align = ALIGN_DOWN(vaddr, PAGE_SIZE);

    paddr = mm_get_paddr(pdir, vaddr_align, PAGE_SHIFT);
    if (paddr == 0)
    {
        return paddr;
    }
    paddr += vaddr - vaddr_align;
    return paddr;
}
