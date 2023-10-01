#include "hmi_user_uart.h"
#include "cmd_queue.h"
#include "usart3.h"
/*!
 *   \brief  发送1个字节
 *   \param  t 发送的字节
 */
void SendChar(uint8_t t)
{
    // usart1_send_byte(t);
    usart3_send_byte(t);
}
