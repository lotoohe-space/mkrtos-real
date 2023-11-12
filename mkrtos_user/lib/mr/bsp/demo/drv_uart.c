/*
 * @copyright (c) 2023, MR Development Team
 *
 * @license SPDX-License-Identifier: Apache-2.0
 *
 * @date 2023-11-10    MacRsh       First version
 */

#include "drv_uart.h"

#ifdef MR_USING_UART

#if !defined(MR_USING_UART1) && !defined(MR_USING_UART2) && !defined(MR_USING_UART3) && !defined(MR_USING_UART4) && !defined(MR_USING_UART5) && !defined(MR_USING_UART6) && !defined(MR_USING_UART7) && !defined(MR_USING_UART8)
#error "Please define at least one UART macro like MR_USING_UART1. Otherwise undefine MR_USING_UART."
#endif

enum uart_drv_index
{
#ifdef MR_USING_UART1
    DRV_INDEX_UART1,
#endif /* MR_USING_UART1 */
#ifdef MR_USING_UART2
    DRV_INDEX_UART2,
#endif /* MR_USING_UART2 */
#ifdef MR_USING_UART3
    DRV_INDEX_UART3,
#endif /* MR_USING_UART3 */
#ifdef MR_USING_UART4
    DRV_INDEX_UART4,
#endif /* MR_USING_UART4 */
#ifdef MR_USING_UART5
    DRV_INDEX_UART5,
#endif /* MR_USING_UART5 */
#ifdef MR_USING_UART6
    DRV_INDEX_UART6,
#endif /* MR_USING_UART6 */
#ifdef MR_USING_UART7
    DRV_INDEX_UART7,
#endif /* MR_USING_UART7 */
#ifdef MR_USING_UART8
    DRV_INDEX_UART8,
#endif /* MR_USING_UART8 */
};

static const char *uart_name[] =
    {
#ifdef MR_USING_UART1
        "uart1",
#endif /* MR_USING_UART1 */
#ifdef MR_USING_UART2
        "uart2",
#endif /* MR_USING_UART2 */
#ifdef MR_USING_UART3
        "uart3",
#endif /* MR_USING_UART3 */
#ifdef MR_USING_UART4
        "uart4",
#endif /* MR_USING_UART4 */
#ifdef MR_USING_UART5
        "uart5",
#endif /* MR_USING_UART5 */
#ifdef MR_USING_UART6
        "uart6",
#endif /* MR_USING_UART6 */
#ifdef MR_USING_UART7
        "uart7",
#endif /* MR_USING_UART7 */
#ifdef MR_USING_UART8
        "uart8",
#endif /* MR_USING_UART8 */
};

static struct drv_uart_data uart_drv_data[] =
    {
#ifdef MR_USING_UART1
        {0,
         0,
         0,
         0,
         0,
         0,
         0,
         0,
         0}
#endif
};

static struct mr_uart uart_dev[mr_array_num(uart_drv_data)];

static int drv_uart_configure(struct mr_uart *uart, struct mr_uart_config *config)
{
    printf("%s:%d\n", __func__, __LINE__);
    return MR_EOK;
}

static ssize_t drv_uart_read(struct mr_uart *uart, uint8_t *buf, size_t size)
{
    printf("%s:%d\n", __func__, __LINE__);
    memcpy(buf, "mkrtos..\n", size);
    return size;
}

static ssize_t drv_uart_write(struct mr_uart *uart, const uint8_t *buf, size_t size)
{
    printf("%s:%d\n", __func__, __LINE__);
    printf("uart write:%s\n", buf);
    return size;
}

static void drv_uart_start_tx(struct mr_uart *uart)
{
    struct drv_uart_data *uart_data = (struct drv_uart_data *)uart->dev.drv->data;
    printf("%s:%d\n", __func__, __LINE__);

    /* Enable TXE */
}

static void drv_uart_stop_tx(struct mr_uart *uart)
{
    struct drv_uart_data *uart_data = (struct drv_uart_data *)uart->dev.drv->data;
    printf("%s:%d\n", __func__, __LINE__);

    /* Disable TXE */
}

static void drv_uart_isr(struct mr_uart *uart)
{
}

#ifdef MR_USING_UART1
void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART1_IRQHandler(void)
{
    drv_uart_isr(&uart_dev[DRV_INDEX_UART1]);
}
#endif /* MR_USING_UART1 */

#ifdef MR_USING_UART2
void USART2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART2_IRQHandler(void)
{
    drv_uart_isr(&uart_dev[DRV_INDEX_UART2]);
}
#endif /* MR_USING_UART2 */

#ifdef MR_USING_UART3
void USART3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART3_IRQHandler(void)
{
    drv_uart_isr(&uart_dev[DRV_INDEX_UART3]);
}
#endif /* MR_USING_UART3 */

#ifdef MR_USING_UART4
void UART4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void UART4_IRQHandler(void)
{
    drv_uart_isr(&uart_dev[DRV_INDEX_UART4]);
}
#endif /* MR_USING_UART4 */

#ifdef MR_USING_UART5
void UART5_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void UART5_IRQHandler(void)
{
    drv_uart_isr(&uart_dev[DRV_INDEX_UART5]);
}
#endif /* MR_USING_UART5 */

#ifdef MR_USING_UART6
void UART6_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void UART6_IRQHandler(void)
{
    drv_uart_isr(&uart_dev[DRV_INDEX_UART6]);
}
#endif /* MR_USING_UART6 */

#ifdef MR_USING_UART7
void UART7_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void UART7_IRQHandler(void)
{
    drv_uart_isr(&uart_dev[DRV_INDEX_UART7]);
}
#endif /* MR_USING_UART7 */

#ifdef MR_USING_UART8
void UART8_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void UART8_IRQHandler(void)
{
    drv_uart_isr(&uart_dev[DRV_INDEX_UART8]);
}
#endif /* MR_USING_UART8 */

static struct mr_uart_ops uart_drv_ops =
    {
        drv_uart_configure,
        drv_uart_read,
        drv_uart_write,
        drv_uart_start_tx,
        drv_uart_stop_tx};

static struct mr_drv uart_drv[mr_array_num(uart_drv_data)] =
    {
#ifdef MR_USING_UART1
        {Mr_Drv_Type_Uart,
         &uart_drv_ops,
         &uart_drv_data[DRV_INDEX_UART1]},
#endif /* MR_USING_UART1 */
#ifdef MR_USING_UART2
        {Mr_Drv_Type_Uart,
         &uart_drv_ops,
         &uart_drv_data[DRV_INDEX_UART2]},
#endif /* MR_USING_UART2 */
#ifdef MR_USING_UART3
        {Mr_Drv_Type_Uart,
         &uart_drv_ops,
         &uart_drv_data[DRV_INDEX_UART3]},
#endif /* MR_USING_UART3 */
#ifdef MR_USING_UART4
        {Mr_Drv_Type_Uart,
         &uart_drv_ops,
         &uart_drv_data[DRV_INDEX_UART4]},
#endif /* MR_USING_UART4 */
#ifdef MR_USING_UART5
        {Mr_Drv_Type_Uart,
         &uart_drv_ops,
         &uart_drv_data[DRV_INDEX_UART5]},
#endif /* MR_USING_UART5 */
#ifdef MR_USING_UART6
        {Mr_Drv_Type_Uart,
         &uart_drv_ops,
         &uart_drv_data[DRV_INDEX_UART6]},
#endif /* MR_USING_UART6 */
#ifdef MR_USING_UART7
        {Mr_Drv_Type_Uart,
         &uart_drv_ops,
         &uart_drv_data[DRV_INDEX_UART7]},
#endif /* MR_USING_UART7 */
#ifdef MR_USING_UART8
        {Mr_Drv_Type_Uart,
         &uart_drv_ops,
         &uart_drv_data[DRV_INDEX_UART8]},
#endif /* MR_USING_UART8 */
};

int drv_uart_init(void)
{
    int index = 0;

    for (index = 0; index < mr_array_num(uart_dev); index++)
    {
        mr_uart_register(&uart_dev[index], uart_name[index], &uart_drv[index]);
    }
    return MR_EOK;
}
MR_INIT_CONSOLE_EXPORT(drv_uart_init);

#endif /* MR_USING_UART */
