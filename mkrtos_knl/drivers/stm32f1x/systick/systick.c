
#include "arch.h"
#include "thread.h"
#include "ipc.h"

static umword_t sys_tick_cnt;

umword_t sys_tick_cnt_get(void)
{
    return sys_tick_cnt;
}

void SysTick_Handler(void)
{
    // 进行上下文切换
    thread_sched();
    sys_tick_cnt++;
    thread_timeout_check();
    timeout_times_tick();
    futex_timeout_times_tick();
}
