#include <types.h>
#include <thread.h>
#include <arch.h>
#include <util.h>
#include <sche_arch.h>
#include <mm_space.h>
#include <task.h>
#define SCHE_DEBUG 0

static void sche_arch_sw_sysregs(thread_t *cur_th, thread_t *next_th);
static void sw_mmu(thread_t *next_thread)
{
    thread_t *cur_th = thread_get_current();
    umword_t p_curr_dir = read_sysreg(vttbr_el2);
    task_t *next_task = (task_t *)(next_thread->task);
    umword_t p_next_dir = (umword_t)mm_space_get_pdir(&next_task->mm_space)->dir;

    assert(next_task);
    if (p_curr_dir != p_next_dir)
    {

        assert(get_sp());
        write_sysreg(p_next_dir | (next_task->mm_space.asid << 48) /*TODO:*/, vttbr_el2); // 切换用户态页表
        _isb();
        // asm volatile("ic iallu");
        // _dsb(ish);

        // asm volatile("tlbi vmalle1");
        // _dsb(ish);
        // _isb();
        // mword_t vttbr;
        // // FIXME: could do a compare for the current VMID before loading
        // // the vttbr and the isb
        // asm volatile(
        // 	"mrs %[vttbr], vttbr_el2\n"
        // 	"msr vttbr_el2, %[asid] \n"
        // 	"isb                    \n"
        // 	"dsb ishst              \n"
        // 	"tlbi ipas2e1, %[ipa]   \n"
        // 	"dsb ish                \n"
        // 	"tlbi vmalle1           \n"
        // 	"dsb ish                \n"
        // 	"msr vttbr_el2, %[vttbr]\n"
        // 	:
        // 	[vttbr] "=&r" (vttbr)
        // 	:
        // 	[ipa] "r" ((unsigned long)va >> 12),
        // 	[asid] "r" (1 << 48)
        // 	:
        // 	"memory");
    }
}
static void shce_arch_sw_sysregs(thread_t *cur_th, thread_t *next_th)
{
    cur_th->sp.hyp.tpidr_el1 = read_sysreg(tpidr_el1);
    cur_th->sp.hyp.sp_el1 = read_sysreg(sp_el1);
    cur_th->sp.hyp.par_el1 = read_sysreg(par_el1);
    cur_th->sp.hyp.hcr_el2 = read_sysreg(hcr_el2);
    cur_th->sp.hyp.cntv_cval_el0 = read_sysreg(cntv_cval_el0);
    cur_th->sp.hyp.cntkctl_el1 = read_sysreg(cntkctl_el1);
    cur_th->sp.hyp.cntv_ctl_el0 = read_sysreg(cntv_ctl_el0);
    cur_th->sp.hyp.elr_el1 = read_sysreg(elr_el1);
    cur_th->sp.hyp.vbar_el1 = read_sysreg(vbar_el1);
    cur_th->sp.hyp.cpacr_el1 = read_sysreg(cpacr_el1);
    cur_th->sp.hyp.spsr_fiq = read_sysreg(spsr_fiq);
    cur_th->sp.hyp.spsr_irq = read_sysreg(spsr_irq);
    cur_th->sp.hyp.spsr_el1 = read_sysreg(spsr_el1);
    cur_th->sp.hyp.spsr_abt = read_sysreg(spsr_abt);
    cur_th->sp.hyp.spsr_und = read_sysreg(spsr_und);
    cur_th->sp.hyp.csselr_el1 = read_sysreg(csselr_el1);

    write_sysreg(next_th->sp.hyp.tpidr_el1, tpidr_el1); // 线程id
    write_sysreg(next_th->sp.hyp.sp_el1, sp_el1);       // el1的sp
    write_sysreg(next_th->sp.hyp.par_el1, par_el1);     // 翻译的物理地址
    write_sysreg(next_th->sp.hyp.hcr_el2, hcr_el2);     // 虚拟化控制寄存器
    write_sysreg(next_th->sp.hyp.cntv_cval_el0, cntv_cval_el0);
    write_sysreg(next_th->sp.hyp.cntkctl_el1, cntkctl_el1);
    write_sysreg(next_th->sp.hyp.cntv_ctl_el0, cntv_ctl_el0);
    write_sysreg(next_th->sp.hyp.elr_el1, elr_el1);     // 异常返回地址寄存器
    write_sysreg(next_th->sp.hyp.vbar_el1, vbar_el1);   // 中断向量表寄存器
    write_sysreg(next_th->sp.hyp.cpacr_el1, cpacr_el1); // 浮点控制
    write_sysreg(next_th->sp.hyp.spsr_fiq, spsr_fiq);
    write_sysreg(next_th->sp.hyp.spsr_irq, spsr_irq);
    write_sysreg(next_th->sp.hyp.spsr_el1, spsr_el1); // 异常返回控制寄存器
    write_sysreg(next_th->sp.hyp.spsr_abt, spsr_abt);
    write_sysreg(next_th->sp.hyp.spsr_und, spsr_und);
    write_sysreg(next_th->sp.hyp.csselr_el1, csselr_el1);
#if IS_ENABLED(SCHE_DEBUG)
    printk("cur hcr_el2:0x%lx next hcr_el2:0x%lx\n", cur_th->sp.hyp.hcr_el2, next_th->sp.hyp.hcr_el2);
#endif
}
static void hype_sche_arch_sw_sysregs(thread_t *cur_th, thread_t *next_th)
{
    if (cur_th->is_vcpu)
    {
        cur_th->sp.sysregs.hstr_el2 = read_sysreg(hstr_el2);
        cur_th->sp.sysregs.vmpidr_el2 = read_sysreg(vmpidr_el2);
        cur_th->sp.sysregs.sp_el0 = read_sysreg(sp_el0);
        cur_th->sp.sysregs.sctlr_el1 = read_sysreg(sctlr_el1);
        cur_th->sp.sysregs.ttbr0_el1 = read_sysreg(ttbr0_el1);
        cur_th->sp.sysregs.ttbr1_el1 = read_sysreg(ttbr1_el1);
        cur_th->sp.sysregs.tcr_el1 = read_sysreg(tcr_el1);
        cur_th->sp.sysregs.esr_el1 = read_sysreg(esr_el1);
        cur_th->sp.sysregs.far_el1 = read_sysreg(far_el1);
        cur_th->sp.sysregs.mair_el1 = read_sysreg(mair_el1);
        cur_th->sp.sysregs.contextidr_el1 = read_sysreg(contextidr_el1);
        cur_th->sp.sysregs.tpidrro_el0 = read_sysreg(tpidrro_el0);
#if IS_ENABLED(SCHE_DEBUG)
        printk("store cur_th sysregs:\n");
        printk("\tsp_el0:0x%lx\n", cur_th->sp.sysregs.sp_el0);
        printk("\tsctlr_el1:0x%lx\n", cur_th->sp.sysregs.sctlr_el1);
        printk("\tttbr0_el1:0x%lx\n", cur_th->sp.sysregs.ttbr0_el1);
        printk("\tttbr1_el1:0x%lx\n", cur_th->sp.sysregs.ttbr1_el1);
        printk("\ttcr_el1:0x%lx\n", cur_th->sp.sysregs.tcr_el1);
        printk("\tesr_el1:0x%lx\n", cur_th->sp.sysregs.esr_el1);
        printk("\tmair_el1:0x%lx\n", cur_th->sp.sysregs.mair_el1);
        printk("\tcontextidr_el1:0x%lx\n", cur_th->sp.sysregs.contextidr_el1);
        printk("\ttpidrro_el0:0x%lx\n", cur_th->sp.sysregs.tpidrro_el0);
#endif
    }
    if (next_th->is_vcpu)
    {
        write_sysreg(next_th->sp.sysregs.hstr_el2, hstr_el2);
        write_sysreg(next_th->sp.sysregs.vmpidr_el2, vmpidr_el2);
        write_sysreg(next_th->sp.sysregs.sp_el0, sp_el0);
        write_sysreg(next_th->sp.sysregs.sctlr_el1, sctlr_el1);
        write_sysreg(next_th->sp.sysregs.ttbr0_el1, ttbr0_el1);
        write_sysreg(next_th->sp.sysregs.ttbr1_el1, ttbr1_el1);
        write_sysreg(next_th->sp.sysregs.tcr_el1, tcr_el1);
        write_sysreg(next_th->sp.sysregs.esr_el1, esr_el1);
        write_sysreg(next_th->sp.sysregs.far_el1, far_el1);
        write_sysreg(next_th->sp.sysregs.mair_el1, mair_el1);
        write_sysreg(next_th->sp.sysregs.contextidr_el1, contextidr_el1);
        write_sysreg(next_th->sp.sysregs.tpidrro_el0, tpidrro_el0);
#if IS_ENABLED(SCHE_DEBUG)
        printk("restore next_th sysregs:\n");
        printk("\tsp_el0:0x%lx\n", next_th->sp.sysregs.sp_el0);
        printk("\tsctlr_el1:0x%lx\n", next_th->sp.sysregs.sctlr_el1);
        printk("\tttbr0_el1:0x%lx\n", next_th->sp.sysregs.ttbr0_el1);
        printk("\tttbr1_el1:0x%lx\n", next_th->sp.sysregs.ttbr1_el1);
        printk("\ttcr_el1:0x%lx\n", next_th->sp.sysregs.tcr_el1);
        printk("\tesr_el1:0x%lx\n", next_th->sp.sysregs.esr_el1);
        printk("\tfar_el1:0x%lx\n", next_th->sp.sysregs.far_el1);
        printk("\tmair_el1:0x%lx\n", next_th->sp.sysregs.mair_el1);
        printk("\tcontextidr_el1:0x%lx\n", next_th->sp.sysregs.contextidr_el1);
        printk("\ttpidrro_el0:0x%lx\n", next_th->sp.sysregs.tpidrro_el0);
#endif
    }
}
void sche_arch_sw_context(void)
{
    sp_info_t *pev_sp;
    scheduler_t *sche = scheduler_get_current();
    thread_t *cur_th = thread_get_current();
    assert(cur_th->magic == THREAD_MAGIC);
    assert(sche->cur_sche);

    sched_t *next = sche->cur_sche;
    thread_t *next_th = container_of(next, thread_t, sche);
    assert(next_th->magic == THREAD_MAGIC);

    // 这里切换页表
    shce_arch_sw_sysregs(cur_th, next_th);
#if IS_ENABLED(CONFIG_VCPU)
    if (cur_th->is_vcpu || next_th->is_vcpu)
    {
        // 如果支持VCPU，则切换系统寄存器等
        hype_sche_arch_sw_sysregs(cur_th, next_th);
        // asm volatile("tlbi alle1is");
        // _dsb(ish);
        // _isb();
    }
#endif
    sw_mmu(next_th); // 切换mmu
    assert(arch_get_current_cpu_id() == cur_th->cpu);

    pev_sp = cpu_switch_to(&cur_th->sp, &next_th->sp); // 切换上下文
    next_th = cur_th;                                  // 当前就是下一个
    cur_th = container_of(pev_sp, thread_t, sp);
    atomic_inc(&cur_th->time_count); /**FIXME:临时处理，代表切换完成*/
}
