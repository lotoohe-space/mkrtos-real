#include "stm32_sys.h"
#include "u_types.h"
#include "u_prot.h"
#include "u_factory.h"
#include "u_hd_man.h"
#include "u_irq_sender.h"
#include "u_mm.h"
#include "u_log.h"
#include "u_local_thread.h"
#include <assert.h>

#define IRQ_THREAD_PRIO 3
#define STACK_SIZE 512
static __attribute__((aligned(8))) uint8_t stack0[STACK_SIZE];
static obj_handler_t obj;

static void *TIM2_IRQHandler(void *arg);

void timer_init(void)
{
    obj = handler_alloc();
    assert(obj != HANDLER_INVALID);
    msg_tag_t tag = factory_create_irq_sender(FACTORY_PROT, vpage_create_raw3(0, 0, obj));
    assert(msg_tag_get_val(tag) >= 0);
    uirq_bind(obj, TIM2_IRQn, u_irq_prio_create(1, 1));

    int ret = thread_create(IRQ_THREAD_PRIO, TIM2_IRQHandler, (umword_t)(stack0 + STACK_SIZE), NULL);
    assert(ret >= 0);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = 100 - 1;                 // 自动重装值（此时改为100ms）
    TIM_TimeBaseStructure.TIM_Prescaler = 720 - 1;              // 时钟预分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     // 时钟分频1
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

static void *TIM2_IRQHandler(void *arg)
{
    while (1)
    {
        msg_tag_t tag = uirq_wait(obj, 0);
        if (msg_tag_get_val(tag) >= 0)
        {
            if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) // 检查TIM3更新中断发生否
            {

                TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
            }
            uirq_ack(obj, TIM2_IRQn);
        }
    }
    return NULL;
}
