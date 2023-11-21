#include "u_factory.h"
#include "u_irq_sender.h"
#include "u_prot.h"
#include "u_hd_man.h"
#include "u_local_thread.h"
#include "u_sleep.h"

#include "wk2xx_hw.h"
#include "spi2.h"
#include "delay.h"
#include "wk2xx.h"
#include "MDM_RTU_Serial.h"
#include <assert.h>
// #include "MDM_RTU_Serial.h"
static obj_handler_t irq_obj;

#define IRQ_THREAD_PRIO 3
#define STACK_SIZE 512
static __attribute__((aligned(8))) uint8_t stack0[STACK_SIZE];
static void *exti_12_irq(void *arg);

void wk2xx_hw_init(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI2_Init();

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOB, GPIO_Pin_6);
    delay_us(100);
    GPIO_SetBits(GPIOB, GPIO_Pin_6);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12);

    EXTI_InitStructure.EXTI_Line = EXTI_Line12;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    // NVIC_Init(&NVIC_InitStructure);
    irq_obj = handler_alloc();
    assert(irq_obj != HANDLER_INVALID);
    msg_tag_t tag = factory_create_irq_sender(FACTORY_PROT, vpage_create_raw3(0, 0, irq_obj));
    assert(msg_tag_get_val(tag) >= 0);
    uirq_bind(irq_obj, EXTI15_10_IRQn, u_irq_prio_create(0, 1));

    int ret = thread_create(IRQ_THREAD_PRIO, exti_12_irq, (umword_t)(stack0 + STACK_SIZE), NULL);
    assert(ret >= 0);
}
u8 wk_recv_buf[4][256] = {0};
static void *exti_12_irq(void *arg)
{
    while (1)
    {
        msg_tag_t tag = uirq_wait(irq_obj, 0);
        if (msg_tag_get_val(tag) >= 0)
        {
            int recv_len = 0;
            if (EXTI_GetFlagStatus(EXTI_Line12) != RESET)
            {
                if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0)
                {
                    u8 intr_reg = WkReadGReg(WK2XXX_GIFR) & 0xf;

                    // 获取那个设备产生的中断
                    switch (intr_reg)
                    {
                    case WK2XXX_UT1IE:
                        recv_len = wk_RxChars(1, wk_recv_buf[0]);
                        break;
                    case WK2XXX_UT2IE:
                        // 485
                        recv_len = wk_RxChars(2, wk_recv_buf[1]);
                        for (int i = 0; i < recv_len; i++)
                        {
                            // MDMSerialRecvByte(wk_recv_buf[1][i]);TODO:
                        }
                        break;
                    case WK2XXX_UT3IE:
                        recv_len = wk_RxChars(3, wk_recv_buf[2]);
                        break;
                    case WK2XXX_UT4IE:
                        // 485
                        recv_len = wk_RxChars(4, wk_recv_buf[3]);
                        for (int i = 0; i < recv_len; i++)
                        {
                            MDMSerialRecvByte(wk_recv_buf[3][i]);
                        }
                        break;
                    }
                }
                // 清除中断位
                EXTI_ClearITPendingBit(EXTI_Line12);
            }
            uirq_ack(irq_obj, EXTI15_10_IRQn);
        }
    }
}
