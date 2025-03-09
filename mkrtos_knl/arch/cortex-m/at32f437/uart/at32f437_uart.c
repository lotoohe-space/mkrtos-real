
#include "mk_sys.h"
#include "types.h"
#include "uart/uart.h"
#include "init.h"
#include "queue.h"
#include <irq.h>
#define PRINT_USARTx USART1
#define PRINT_USART_CLK CRM_USART1_PERIPH_CLOCK

#define PRINT_USART_TX_GPIO_CLK CRM_GPIOA_PERIPH_CLOCK
#define PRINT_USART_TX_GPIO_PIN GPIO_PINS_9
#define PRINT_USART_RX_GPIO_PIN GPIO_PINS_10
#define PRINT_USART_TX_GPIO_PINS_SOURCE GPIO_PINS_SOURCE9
#define PRINT_USART_RX_GPIO_PINS_SOURCE GPIO_PINS_SOURCE10
#define PRINT_USART_TX_GPIO_PORT GPIOA
#define PRINT_USART_RX_GPIO_PORT GPIOA
#define PRINT_USART_TX_GPIO_MUX GPIO_MUX_7
#define PRINT_USART_RX_GPIO_MUX GPIO_MUX_7

static uart_t uart = {
    .baud = 115200};

uart_t *uart_get_global(void)
{
    return &uart;
}
#define QUEUE_LEN 129
static queue_t queue;
static uint8_t queue_data[QUEUE_LEN];
static bool_t uart_is_init;
void uart_tigger(irq_entry_t *irq)
{
    if (usart_interrupt_flag_get(PRINT_USARTx, USART_RDBF_FLAG) != RESET)
    {
        /* read one byte from the receive data register */
        q_enqueue(&queue, usart_data_receive(PRINT_USARTx));
        usart_interrupt_enable(PRINT_USARTx, USART_IDLE_INT, TRUE);
    }
    if (usart_interrupt_flag_get(PRINT_USARTx, USART_IDLEF_FLAG) != RESET)
    {
        if (irq->irq->wait_thread && thread_get_status(irq->irq->wait_thread) == THREAD_SUSPEND)
        {
            thread_ready_remote(irq->irq->wait_thread, TRUE);
        }
        usart_interrupt_enable(PRINT_USARTx, USART_IDLE_INT, FALSE);
    }
}

void uart_init(void)
{
    q_init(&queue, queue_data, QUEUE_LEN);
    gpio_init_type gpio_init_struct;

    /* enable the uart1 and gpio clock */
    crm_periph_clock_enable(PRINT_USART_CLK, TRUE);
    crm_periph_clock_enable(PRINT_USART_TX_GPIO_CLK, TRUE);

    gpio_default_para_init(&gpio_init_struct);

    /* configure the uart1 tx pin */
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_init_struct.gpio_pins = PRINT_USART_TX_GPIO_PIN | PRINT_USART_RX_GPIO_PIN;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(PRINT_USART_TX_GPIO_PORT, &gpio_init_struct);
    gpio_pin_mux_config(PRINT_USART_TX_GPIO_PORT, PRINT_USART_TX_GPIO_PINS_SOURCE, PRINT_USART_TX_GPIO_MUX);
    gpio_pin_mux_config(PRINT_USART_RX_GPIO_PORT, PRINT_USART_RX_GPIO_PINS_SOURCE, PRINT_USART_RX_GPIO_MUX);

    nvic_priority_group_config(NVIC_PRIORITY_GROUP_2);
    nvic_irq_enable(USART1_IRQn, 0, 0);
    /* configure uart param */
    usart_init(PRINT_USARTx, 921600, USART_DATA_8BITS, USART_STOP_1_BIT);
    usart_transmitter_enable(PRINT_USARTx, TRUE);
    usart_receiver_enable(PRINT_USARTx, TRUE);

    usart_interrupt_enable(PRINT_USARTx, USART_RDBF_INT, TRUE);
    usart_interrupt_enable(PRINT_USARTx, USART_IDLE_INT, TRUE);
    
    usart_enable(PRINT_USARTx, TRUE);
   
    uart_is_init=1;
}
INIT_HIGH_HAD(uart_init);

void uart_set(uart_t *uart)
{
    assert(0);
    // uart_set_baud(USART1, uart->baud);
    // uart_set_parity(USART1, uart->parity);
}
void uart_putc(uart_t *uart, int data)
{
    if (!uart_is_init) {
        return ;
    }
    while (usart_flag_get(USART1, USART_TDBE_FLAG) == RESET)
        ;
    usart_data_transmit(USART1, (uint16_t)(data));
    while (usart_flag_get(USART1, USART_TDC_FLAG) == RESET)
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
