
#include "arch.h"
#include "thread.h"
#include "futex.h"
static umword_t sys_tick_cnt;

umword_t sys_tick_cnt_get(void)
{
    return sys_tick_cnt;
}
#include <asm/arm_local_reg.h>

extern void handle_timer_irq(void);
void SysTick_Handler(void)
{
    handle_timer_irq(); // TODO:定时器的处理应该被分流，这里处理还有点问题，而且最好采用通用定时器
    // 进行上下文切换
    sys_tick_cnt++;
    thread_timeout_check(1);
    futex_timeout_times_tick();
    thread_sched();
}
