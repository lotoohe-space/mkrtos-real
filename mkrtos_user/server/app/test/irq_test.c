
#include "u_types.h"
#include "u_prot.h"
#include "u_factory.h"
#include "u_hd_man.h"
#include "u_irq_sender.h"
#include "u_mm.h"
#include "u_log.h"
#include <assert.h>
#include <stm32_sys.h>
#include <stdio.h>
void timer_init(void)
{
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

void irq_test(void)
{
    obj_handler_t obj = handler_alloc();
    assert(obj != HANDLER_INVALID);
    msg_tag_t tag = factory_create_irq_sender(FACTORY_PROT, vpage_create_raw3(0, 0, obj));
    assert(msg_tag_get_val(tag) >= 0);
    uirq_bind(obj, TIM2_IRQn, 0);

    tag = mm_align_alloc(MM_PROT, (void *)0x40000000, 0x50000000 - 0x40000000);
    assert(msg_tag_get_val(tag) >= 0);

    timer_init();

    while (1)
    {
        static int i = 0;
        uirq_wait(obj);
        ulog_write_str(LOG_PROT, ".");
        uirq_ack(obj, TIM2_IRQn);
        if ((i++ % 20) == 0)
        {
            ulog_write_str(LOG_PROT, "\n");
        }
        if (i >= 100)
        {
            break;
        }
    }
    ulog_write_str(LOG_PROT, "irq test success.\n");
    handler_free_umap(obj);
}