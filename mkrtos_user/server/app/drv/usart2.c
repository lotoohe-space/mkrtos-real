#include "usart2.h"
#include "stm32_sys.h"
#include "u_factory.h"
#include "u_irq_sender.h"
#include "u_prot.h"
#include "u_hd_man.h"
#include "u_local_thread.h"
#include "u_sleep.h"
#include <assert.h>
#include <fcntl.h>

static obj_handler_t irq_obj;

#define IRQ_THREAD_PRIO 3
#define STACK_SIZE 512
static __attribute__((aligned(8))) uint8_t stack0[STACK_SIZE];
static void *USART2_IRQHandler(void *arg);

void init_usart2(u32 BaudRate)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStruct;
    /* Enable GPIO clock */
    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_3);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    /* Configure USARTz */
    USART_Init(USART2, &USART_InitStructure);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART2, ENABLE);

    // 设置中断优先级
    // NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
    // NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    // NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    // NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
    // NVIC_Init(&NVIC_InitStruct);

    irq_obj = handler_alloc();
    assert(irq_obj != HANDLER_INVALID);
    msg_tag_t tag = factory_create_irq_sender(FACTORY_PROT, vpage_create_raw3(0, 0, irq_obj));
    assert(msg_tag_get_val(tag) >= 0);
    uirq_bind(irq_obj, USART2_IRQn, u_irq_prio_create(1, 1));

    int ret = thread_create(IRQ_THREAD_PRIO, USART2_IRQHandler, (umword_t)(stack0 + STACK_SIZE), NULL);
    assert(ret >= 0);
}

void usart2_send_byte(u8 byte)
{
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
        ;
    USART_SendData(USART2, byte);
}
void usart2_send_bytes(u8 *bytes, int len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        usart2_send_byte(bytes[i]);
    }
}
void usart2_send_string(char *string)
{
    while (*string)
    {
        usart2_send_byte(*string++);
    }
}

u8 usart2_data[USART2_RECV_CACHE_MAX_LEN] = {0};
u16 usart2_count = 0;
u8 usart2_flag = 0;
u8 usart2_step = 0;
u8 usart2_over_timer = 0;

static void *USART2_IRQHandler(void *arg)
{
    while (1)
    {
        msg_tag_t tag = uirq_wait(irq_obj, 0);
        if (msg_tag_get_val(tag) >= 0)
        {
            if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
            {
                uint8_t data = USART_ReceiveData(USART2);
                usart2_over_timer = 0;
                if (usart2_flag == 0)
                {
                    usart2_data[usart2_count] = data;

                    if (usart2_data[usart2_count] == '\r')
                    {
                        usart2_step = 1;
                    }
                    else if (usart2_data[usart2_count] == '\n')
                    {
                        usart2_step = 2;
                    }
                    else
                    {
                        usart2_step = 0;
                    }
                    usart2_count++;
                    if (usart2_count >= USART2_RECV_CACHE_MAX_LEN || usart2_step == 2)
                    {
                        usart2_flag = 1; // 接收完成
                        usart2_loop();
                    }
                }
            }
            uirq_ack(irq_obj, USART2_IRQn);
        }
    }
}

void usart2_over_time_check(void)
{
    usart2_over_timer++;
    if (usart2_over_timer >= 15 && usart2_count != 0)
    {
        usart2_flag = 1;
        usart2_over_timer = 0;
    }
}

usart_recv_data_fun usart_recv_data_cb = 0x00;

void usart2_set_recv_data_cb(usart_recv_data_fun usart_recv_fun)
{
    usart_recv_data_cb = usart_recv_fun;
}

void usart2_loop(void)
{
    if (usart2_flag == 1)
    {
        if (usart2_count > 0)
        {
            if (usart_recv_data_cb != 0x00)
            {
                usart_recv_data_cb(usart2_data, usart2_count);
            }
            usart2_count = 0;
            usart2_flag = 0;
        }
    }
}
