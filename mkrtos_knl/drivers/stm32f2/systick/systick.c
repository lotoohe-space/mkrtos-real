
#include "arch.h"
#include "thread.h"
#include "futex.h"
#include "irq.h"
static umword_t sys_tick_cnt;

umword_t sys_tick_cnt_get(void)
{
    return sys_tick_cnt;
}

void SysTick_Handler(void)
{
    // 进行上下文切换
    thread_sched(TRUE);
    sys_tick_cnt++;
    thread_timeout_check(1);
    futex_timeout_times_tick();
    #if 0
    extern void uart_check_timeover(irq_entry_t * irq);
    uart_check_timeover(irq_get(LOG_INTR_NO));
    #endif
}
