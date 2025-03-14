
#include "mk_sys.h"
#include "types.h"
#include "uart/uart.h"
#include "init.h"
#include "queue.h"
#include <irq.h>
#define CR3_CLEAR_MASK ((uint16_t)(USART_CR3_RTSE | USART_CR3_CTSE))

static void uart_set_word_len(USART_TypeDef *USARTx, uint16_t len)
{
    uint32_t tmpreg;

    tmpreg = USARTx->CR1;

    /* Clear M, PCE, PS, TE and RE bits */
    tmpreg &= (uint32_t) ~((uint32_t)USART_CR1_M);

    /* Configure the USART Word Length, Parity and mode:
        Set the M bits according to USART_WordLength value
        Set PCE and PS bits according to USART_Parity value
        Set TE and RE bits according to USART_Mode value */
    tmpreg |= (uint32_t)len;

    /* Write to USART CR1 */
    USARTx->CR1 = (uint16_t)tmpreg;
}
static void uart_set_baud(USART_TypeDef *USARTx, uint32_t baud)
{
    uint32_t tmpreg = 0x00, apbclock = 0x00;
    RCC_ClocksTypeDef RCC_ClocksStatus;

    tmpreg = USART2->BRR;
    uint32_t integerdivider = 0x00;
    uint32_t fractionaldivider = 0x00;

    RCC_GetClocksFreq(&RCC_ClocksStatus);

    if ((USARTx == USART2))
    {
        apbclock = RCC_ClocksStatus.PCLK2_Frequency;
    }
    else
    {
        apbclock = RCC_ClocksStatus.PCLK1_Frequency;
    }

    /* Determine the integer part */
    if ((USARTx->CR1 & USART_CR1_OVER8) != 0)
    {
        /* Integer part computing in case Oversampling mode is 8 Samples */
        integerdivider = ((25 * apbclock) / (2 * (baud)));
    }
    else /* if ((USARTx->CR1 & USART_CR1_OVER8) == 0) */
    {
        /* Integer part computing in case Oversampling mode is 16 Samples */
        integerdivider = ((25 * apbclock) / (4 * (baud)));
    }
    tmpreg = (integerdivider / 100) << 4;

    /* Determine the fractional part */
    fractionaldivider = integerdivider - (100 * (tmpreg >> 4));

    /* Implement the fractional part in the register */
    if ((USARTx->CR1 & USART_CR1_OVER8) != 0)
    {
        tmpreg |= ((((fractionaldivider * 8) + 50) / 100)) & ((uint8_t)0x07);
    }
    else /* if ((USARTx->CR1 & USART_CR1_OVER8) == 0) */
    {
        tmpreg |= ((((fractionaldivider * 16) + 50) / 100)) & ((uint8_t)0x0F);
    }

    USARTx->BRR &= ((~0UL) << 16);
    USARTx->BRR |= (uint16_t)tmpreg;
}
static void uart_set_stop_bit(USART_TypeDef *USARTx, uint16_t stop_bits)
{
    uint32_t tmpreg;

    tmpreg = USARTx->CR2;

    /* Clear STOP[13:12] bits */
    tmpreg &= (uint32_t) ~((uint32_t)USART_CR2_STOP);

    /* Configure the USART Stop Bits, Clock, CPOL, CPHA and LastBit :
    Set STOP[13:12] bits according to USART_StopBits value */
    tmpreg |= (uint32_t)stop_bits;

    /* Write to USART CR2 */
    USARTx->CR2 = (uint16_t)tmpreg;
}
static void uart_set_parity(USART_TypeDef *USARTx, uint16_t parity)
{
    uint32_t tmpreg;

    tmpreg = USARTx->CR1;

    /* Clear M, PCE, PS, TE and RE bits */
    tmpreg &= (uint32_t) ~((uint32_t)USART_CR1_PCE | USART_CR1_PS);

    /* Configure the USART Word Length, Parity and mode:
    Set the M bits according to USART_WordLength value
    Set PCE and PS bits according to USART_Parity value
    Set TE and RE bits according to USART_Mode value */
    tmpreg |= (uint32_t)parity;

    /* Write to USART CR1 */
    USARTx->CR1 = (uint16_t)tmpreg;
}
static void uart_hardware_flow_rts(USART_TypeDef *USARTx, uint16_t flow)
{
    uint32_t tmpreg;

    tmpreg = USARTx->CR3;

    /* Clear CTSE and RTSE bits */
    tmpreg &= (uint32_t) ~((uint32_t)CR3_CLEAR_MASK);

    /* Configure the USART HFC :
    Set CTSE and RTSE bits according to USART_HardwareFlowControl value */
    tmpreg |= flow;

    /* Write to USART CR3 */
    USARTx->CR3 = (uint16_t)tmpreg;
}
static uart_t uart = {
    .baud = 115200};

uart_t *uart_get_global(void)
{
    return &uart;
}
#define QUEUE_LEN 129
static queue_t queue;
static uint8_t queue_data[QUEUE_LEN];
void uart_tigger(irq_entry_t *irq)
{
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {

        q_enqueue(&queue, USART_ReceiveData(USART2));

        if (irq->irq->wait_thread && thread_get_status(irq->irq->wait_thread) == THREAD_SUSPEND)
        {
            thread_ready_remote(irq->irq->wait_thread, TRUE);
        }
        // 清除中断标志位
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}

void uart_init(void)
{
    q_init(&queue, queue_data, QUEUE_LEN);

    // GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能USART1，GPIOA时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); // 使能USART1，GPIOA时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ;  			// GPIOA-PIN_2  USART2-TX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;         // 复用推挽输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				// GPIOA-PIN_3  USART2-RX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	// 浮空输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);

    // Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        // 子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                           // 根据指定的参数初始化VIC寄存器

    // USART 初始化设置

    USART_InitStructure.USART_BaudRate = 115200;                                     // 串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     // 字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          // 一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                             // 无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // 收发模式

    USART_Init(USART2, &USART_InitStructure);      // 初始化串口1
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // 开启串口接受中断
    USART_Cmd(USART2, ENABLE);                     // 使能串口1
}
INIT_HIGH_HAD(uart_init);

void uart_set(uart_t *uart)
{
    uart_set_baud(USART2, uart->baud);
    // uart_set_parity(USART2, uart->parity);
}
void uart_putc(uart_t *uart, int data)
{
    USART_SendData(USART2, (uint8_t)data);

    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
        ;
}
int uart_getc(uart_t *uart)
{
    uint8_t e;
    umword_t status;

    status = cpulock_lock();
    if (q_dequeue(&queue, &e) >= 0)
    {
        cpulock_set(status);
        return e;
    }
    cpulock_set(status);
    return -1;
}
int uart_get_len(uart_t *uart)
{
    return q_queue_len(&queue);
}
