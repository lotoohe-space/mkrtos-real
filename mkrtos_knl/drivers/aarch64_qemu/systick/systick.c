
#include "arch.h"
#include "thread.h"
#include "futex.h"
#include <asm/arm_local_reg.h>
#include <arm_gicv2.h>
#include <sche_arch.h>
#include <timer.h>
#include <pre_cpu.h>
static PER_CPU(umword_t, sys_tick_cnt);

umword_t sys_tick_cnt_get(void)
{
    return (*((umword_t *)pre_cpu_get_var_cpu(0, &sys_tick_cnt)));
}
umword_t sys_tick_cnt_get_current(void)
{

    return (*((umword_t *)pre_cpu_get_current_cpu_var(&sys_tick_cnt)));
}
void systick_handler(irq_entry_t *irq)
{
    mword_t status = cpulock_lock();
    (*((umword_t *)pre_cpu_get_current_cpu_var(&sys_tick_cnt)))++;
    thread_timeout_check(1);
    futex_timeout_times_tick();
    gic2_eoi_irq(arm_gicv2_get_global(), SYSTICK_INTR_NO);
    handle_timer_irq();

    thread_sched(TRUE);
    cpulock_set(status);
}
