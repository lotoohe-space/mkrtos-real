
#include "uart5.h"
#include "cmd_queue.h"
#include "u_factory.h"
#include "u_irq_sender.h"
#include "u_prot.h"
#include "u_hd_man.h"
#include "u_local_thread.h"
#include "u_sleep.h"
#include "MDM_RTU_Serial.h"
#include <assert.h>
#include <fcntl.h>
static obj_handler_t irq_obj;

#define IRQ_THREAD_PRIO 3
#define STACK_SIZE 512
static __attribute__((aligned(8))) uint8_t stack0[STACK_SIZE];

static void *UART5_IRQHandler(void *arg);

void init_uart5(u32 baudRate)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE); // 使能GPIOC时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);                        // 使能串口3时钟

    USART_DeInit(UART5);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 设置PA2为复用推挽输出
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 设置PA3为浮空输入
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_SetBits(GPIOD, GPIO_Pin_2);

    USART_InitStructure.USART_BaudRate = baudRate;                                  // 设置串口波特率为115200
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     // 字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          // 1个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                             // 无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控制
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;                 // 发送和接收模式
    USART_Init(UART5, &USART_InitStructure);                                        // 初始化串口

    // NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
    // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级为1
    // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        // 子优先级为1
    // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQ通道使能
    // NVIC_Init(&NVIC_InitStructure);                           // 中断优先级初始化

    USART_Cmd(UART5, ENABLE);                     // 使能串口
    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE); // 开启中断
    USART_ClearITPendingBit(UART5, USART_IT_RXNE);

    irq_obj = handler_alloc();
    assert(irq_obj != HANDLER_INVALID);
    msg_tag_t tag = factory_create_irq_sender(FACTORY_PROT, vpage_create_raw3(0, 0, irq_obj));
    assert(msg_tag_get_val(tag) >= 0);
    uirq_bind(irq_obj, UART5_IRQn, (1 << 16) | 1);

    int ret = thread_create(IRQ_THREAD_PRIO, UART5_IRQHandler, (umword_t)(stack0 + STACK_SIZE), NULL);
    assert(ret >= 0);
}
static void *UART5_IRQHandler(void *arg)
{
    while (1)
    {
        msg_tag_t tag = uirq_wait(irq_obj, 0);
        if (msg_tag_get_val(tag) >= 0)
        {
            if (USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
            {
                USART_ClearITPendingBit(UART5, USART_IT_RXNE); // 清除中断标志
                uint8_t data = USART_ReceiveData(UART5);

                MDMSerialRecvByte(data);
            }
            uirq_ack(irq_obj, UART5_IRQn);
        }
        // u_sleep_ms(1);
    }
    return NULL;
}
void uart5_send_byte(u8 byte)
{
    while (USART_GetFlagStatus(UART5, USART_FLAG_TXE) == RESET)
        ;
    USART_SendData(UART5, byte);
    while (USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET)
        ;
}
void uart5_send_bytes(u8 *bytes, int len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        uart5_send_byte(bytes[i]);
    }
}
void uart5_send_string(char *string)
{
    while (*string)
    {
        uart5_send_byte(*string++);
    }
}
