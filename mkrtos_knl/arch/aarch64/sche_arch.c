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

    assert(next_task);
    if (p_curr_dir != p_next_dir)
    {

        assert(get_sp());
        // printk("ttbr:0x%lx\n", p_next_dir | (next_task->mm_space.asid << 48));
        // _dsb(sy);
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
    sw_mmu(next_th);
    assert(arch_get_current_cpu_id() == cur_th->cpu);
    // printk("0 sw cpu:%d next_th:0x%lx\n", arch_get_current_cpu_id(), cur_th);
    // 切换上下文
    pev_sp = cpu_switch_to(&cur_th->sp, &next_th->sp);
    cur_th = container_of(pev_sp, thread_t, sp);
    atomic_inc(&cur_th->time_count);/**FIXME:临时处理，代表切换完成*/
    // printk("sw cpu:%d next_th:0x%lx\n", arch_get_current_cpu_id(), cur_th);
}
