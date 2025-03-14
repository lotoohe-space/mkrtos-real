
#include "mk_sys.h"
#include "types.h"
#include "uart/uart.h"
#include "init.h"
#include "queue.h"
#include <arch.h>
#include <irq.h>

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
    uint32_t chr;

    if (UART_INTStat(UART0, UART_IT_RX_THR | UART_IT_RX_TOUT))
    {
        while (UART_IsRXFIFOEmpty(UART0) == 0)
        {
            if (UART_ReadByte(UART0, &chr) == 0)
            {
                q_enqueue(&queue, chr);

                if (irq->irq->wait_thread && thread_get_status(irq->irq->wait_thread) == THREAD_SUSPEND)
                {
                    thread_ready_remote(irq->irq->wait_thread, TRUE);
                }
            }
        }

        if (UART_INTStat(UART0, UART_IT_RX_TOUT))
        {
            UART_INTClr(UART0, UART_IT_RX_TOUT);

            // GPIO_InvBit(GPIOA, PIN5);
        }
    }
}

void uart_init(void)
{
    q_init(&queue, queue_data, QUEUE_LEN);

    UART_InitStructure UART_initStruct;

    PORT_Init(PORTM, PIN0, PORTM_PIN0_UART0_RX, 1);
    PORT_Init(PORTM, PIN1, PORTM_PIN1_UART0_TX, 0);

    UART_initStruct.Baudrate = 115200;
    UART_initStruct.DataBits = UART_DATA_8BIT;
    UART_initStruct.Parity = UART_PARITY_NONE;
    UART_initStruct.StopBits = UART_STOP_1BIT;
    UART_initStruct.RXThreshold = 3;
    UART_initStruct.RXThresholdIEn = 1;
    UART_initStruct.TXThreshold = 3;
    UART_initStruct.TXThresholdIEn = 0;
    UART_initStruct.TimeoutTime = 10;
    UART_initStruct.TimeoutIEn = 1;
    UART_Init(UART0, &UART_initStruct);
    UART_Open(UART0);
}
INIT_HIGH_HAD(uart_init);

void uart_set(uart_t *uart)
{
    UART_SetBaudrate(UART0, uart->baud);
    // uart_set_baud(USART1, uart->baud);
    // uart_set_parity(USART1, uart->parity);
}
void uart_putc(uart_t *uart, int data)
{
    UART_WriteByte(UART0, data);

    while (UART_IsTXBusy(UART0))
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
