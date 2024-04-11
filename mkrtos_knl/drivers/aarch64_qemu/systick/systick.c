
#include "arch.h"
#include "thread.h"
#include "futex.h"
#include <asm/arm_local_reg.h>
#include <arm_gicv2.h>
#include <sche_arch.h>
#include <timer.h>
static umword_t sys_tick_cnt;

umword_t sys_tick_cnt_get(void)
{
    return sys_tick_cnt;
}

void systick_handler(irq_entry_t *irq)
{
    handle_timer_irq();
    gic2_eoi_irq(arm_gicv2_get_global(), SYSTICK_INTR_NO);
    // 进行上下文切换
    sys_tick_cnt++;
    thread_timeout_check(1);
    futex_timeout_times_tick();
    if (thread_sched(FALSE))
    {
        sche_arch_sw_context();
    }
}
