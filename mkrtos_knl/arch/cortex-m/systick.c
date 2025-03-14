
#include "arch.h"
#include "thread.h"
#include "futex.h"
#include "irq.h"
#include "thread_knl.h"
#include "sleep.h"
static umword_t sys_tick_cnt;

umword_t sys_tick_cnt_get(void)
{
    return sys_tick_cnt;
}

void SysTick_Handler(void)
{
    // 进行上下文切换
    if (!thread_sched(TRUE))
    {
        atomic_inc(&thread_get_current()->time_count);
    }
    sys_tick_cnt++;
#if 0
    thread_timeout_check(1);
#endif
    thread_check_timeout();
    futex_timeout_times_tick();
    thread_calc_cpu_usage();
#if 0
    extern void uart_check_timeover(irq_entry_t * irq);
    uart_check_timeover(irq_get(LOG_INTR_NO));
#endif
}
