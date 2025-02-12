
#include "arch.h"
#include "thread.h"
#include "futex.h"
#include "thread_knl.h"
static umword_t sys_tick_cnt;

umword_t sys_tick_cnt_get(void)
{
    return sys_tick_cnt;
}

void SysTick_Handler(void)
{
    umword_t status = cpulock_lock();
    // 进行上下文切换
    if (!thread_sched(TRUE))
    {
        atomic_inc(&thread_get_current()->time_count);
    }
    sys_tick_cnt++;
    thread_timeout_check(1);
    futex_timeout_times_tick();
    cpulock_set(status);
    thread_calc_cpu_usage();
}
