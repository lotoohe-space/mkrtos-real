
#include "arch.h"
#include "thread.h"

void SysTick_Handler(void)
{
    // 进行上下文切换
    thread_sched();
}
