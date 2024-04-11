#include <types.h>
#include <thread.h>
#include <arch.h>
#include <util.h>
#include <sche_arch.h>
#include <mm_space.h>
#include <task.h>
static void sw_mmu(thread_t *next_thread)
{
    umword_t p_curr_dir = read_sysreg(vttbr_el2);
    task_t *next_task = (task_t *)(next_thread->task);
    umword_t p_next_dir = (umword_t)mm_space_get_pdir(&next_task->mm_space)->dir;

    if (p_curr_dir != p_next_dir)
    {
        _dsb(sy);
        write_sysreg(p_next_dir | (next_task->mm_space.asid << 48) /*TODO:*/, vttbr_el2); // 切换用户态页表
        _dsb(ish);
        _isb();
        // printk("asid:%lx.\n", (next_task->mm_space.asid << 48));
        // asm volatile("ic iallu");

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
void sche_arch_sw_context(void)
{
    scheduler_t *sche = scheduler_get_current();
    thread_t *cur_th = thread_get_current();

    sched_t *next = sche->cur_sche;
    thread_t *next_th = container_of(next, thread_t, sche);
    // 这里切换页表
    sw_mmu(next_th);
    // 变成了next的sp
    cpu_switch_to(&cur_th->sp, &next_th->sp);
}
