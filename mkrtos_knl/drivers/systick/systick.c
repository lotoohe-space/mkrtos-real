
#include "arch.h"
#include "thread.h"
#include "ipc.h"
void SysTick_Handler(void)
{
    // 进行上下文切换
    thread_sched();
    timeout_times_tick();
}
