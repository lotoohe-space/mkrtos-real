/*
 * uart.c
 *
 *  Created on: 2022年8月6日
 *      Author: Administrator
 */

#include <mkrtos/tty.h>
#include <config.h>
#include <arch/isr.h>
#include <stm32f2xx.h>



void uart_set_word_len(USART_TypeDef* USARTx, uint16_t len);
void uart_set_baud(USART_TypeDef* USARTx, uint32_t baud);
void uart_set_stop_bit(USART_TypeDef* USARTx, uint16_t stop_bits);
void uart_set_parity(USART_TypeDef* USARTx, uint16_t parity);
void uart_hardware_flow_rts(USART_TypeDef* USARTx, uint16_t flow);

static uint8_t init_flag = 0;
static struct tty_struct *curr_tty = NULL;
static struct serial_struct curr_ss;
static void uart_close(struct tty_struct *tty, struct file *filp);
static int32_t uart_write(struct tty_struct *tty);
static int32_t uart_ioctl(struct tty_struct *tty, struct file *file, uint32_t cmd,
		uint32_t arg);
int32_t uart_open(struct tty_struct *tty, struct file *filp);

static struct dev_ops uart_ops = {
		.open = uart_open,
		.close = uart_close,
		.write = uart_write,
		.ioctl = uart_ioctl
};

static void uart_close(struct tty_struct *tty, struct file *filp) {
	curr_tty = NULL;
	USART_DeInit(USART1);
	init_flag = 0;
}
static int32_t uart_write(struct tty_struct *tty) {
	int res;
	uint8_t r;
	int w_len = 0;
	if (!q_length(&tty->w_queue)) {
		return w_len;
	}
	while ((res = q_get(&tty->w_queue, &r)) >= 0) {
		if (tty->line_no == 0) {
			  USART_SendData(USART1, (uint8_t) r);

			/* Loop until the end of transmission */
			while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
			{}
		}
		w_len++;
	}
	return w_len;
}
static void uart_set_par(uint32_t usart_periph, int32_t csize, int32_t cstopb,
		int32_t par) {
	uint32_t w_tmp = 0;

	switch (csize) {
	// case CS5:
	// 	//uart_set_word_len(usart_periph, 5);
	// 	break;
	// case CS6:
	// 	//uart_set_word_len(usart_periph, 6);
	// 	break;
	// case CS7:
	// 	uart_set_word_len(usart_periph, 7);
	// 	break;
	case CS8:
		uart_set_word_len(usart_periph, USART_WordLength_8b);
		break;
	}

	switch (cstopb) {
	case 0:
		uart_set_stop_bit(usart_periph, USART_StopBits_1);
		break;
	case 1:
		uart_set_stop_bit(usart_periph, USART_StopBits_0_5);
		break;
	case 2:
		uart_set_stop_bit(usart_periph, USART_StopBits_2);
	break;
	case 3:
		uart_set_stop_bit(usart_periph, USART_StopBits_1_5);
	break;
	}
	switch (par) {
	case 0:
		uart_set_parity(usart_periph, USART_Parity_No);
		break;
	case 1:
		uart_set_parity(usart_periph, USART_Parity_Even);
		break;
	case 2:
		uart_set_parity(usart_periph, USART_Parity_Odd);
		break;
	}
}
static int32_t uart_ioctl(struct tty_struct *tty, struct file *file, uint32_t cmd,
		uint32_t arg) {
	struct serial_struct *s_s = (struct serial_struct*) arg;
	int line_no;
	line_no = tty->line_no;
	if (line_no != 0) {
		return -ENOSYS;
	}
	switch (cmd) {
	case TTY_IO_SET:
		if (F_C_BAUD(s_s->c_cflag)) {
			switch (F_C_BAUD(s_s->c_cflag)) {
			case B0:
			case B50:
			case B75:
			case B110:
			case B134:
			case B150:
			case B200:
			case B300:
			case B600:
			case B1200:
			case B1800:
				return -ENOSYS;
			case B2400:
				uart_set_baud(USART1, 2400);
				break;
			case B4800:
				uart_set_baud(USART1, 4800);
				break;
			case B9600:
				uart_set_baud(USART1, 9600);
				break;
			case B19200:
				uart_set_baud(USART1, 19200);
				break;
			case B38400:
				uart_set_baud(USART1, 38400);
				break;
			case B57600:
				uart_set_baud(USART1, 57600);
				break;
		    case B115200:
				uart_set_baud(USART1, 115200);
				break;
			case B230400:
				uart_set_baud(USART1, 230400);
				break;
			case B460800:
				uart_set_baud(USART1, 460800);
				break;
			case B500000:
				uart_set_baud(USART1, 500000);
				break;
			case B576000:
				uart_set_baud(USART1, 576000);
				break;
			case B921600:
				uart_set_baud(USART1, 921600);
				break;
			case B1000000:
				uart_set_baud(USART1, 1000000);
				break;
			case B1152000:
				uart_set_baud(USART1, 1152000);
				break;
			case B1500000:
				uart_set_baud(USART1, 1500000);
				break;
			case B2000000:
				uart_set_baud(USART1, 2000000);
				break;
			case B2500000:
				uart_set_baud(USART1, 2500000);
				break;
			case B3000000:
				uart_set_baud(USART1, 3000000);
				break;
			case B3500000:
				uart_set_baud(USART1, 3500000);
				break;
			case B4000000:
				uart_set_baud(USART1, 4000000);
				break;
			}
		}
		uart_set_par(USART1, F_C_CSIZE(s_s->c_cflag), F_C_CSTOPB(s_s->c_cflag) ? 1 : 0,
				F_C_PARENB(s_s->c_cflag) ? 1 : 0);
		uart_hardware_flow_rts(USART1,
				F_C_CRTSCTS(s_s->c_cflag) ? USART_HardwareFlowControl_RTS_CTS : USART_HardwareFlowControl_None);
		uart_hardware_flow_rts(USART1,
				F_C_CRTSCTS(s_s->c_cflag) ? USART_HardwareFlowControl_RTS_CTS : USART_HardwareFlowControl_None);
		curr_ss.c_cflag = s_s->c_cflag;
		break;
	case TTY_IO_GET:
		s_s->c_cflag = curr_ss.c_cflag;
		break;

	}
	return 0;
}

static void USART1_IRQHandler(void)
{
	struct tty_queue *w_tmp;
	w_tmp = &curr_tty->r_queue;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        /* receive data */
        uint8_t data = USART_ReceiveData(USART1);
        q_add(w_tmp, data);
        wake_up(curr_tty->r_wait);
    }
}
int32_t uart_open(struct tty_struct *tty, struct file *filp) {

	if (init_flag) {
		return 0;
	}
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_10;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);

	reg_isr_func(USART1_IRQHandler, USART1_IRQn + 1, 0);
	curr_tty = tty;
	curr_ss.c_cflag = tty->termios.c_cflag;

	tty->d_ops = &uart_ops;
	//设置参数
	uart_ioctl(tty, filp, TTY_IO_SET, (uint32_t)(&curr_ss));

	init_flag = 1;
	return 0;
}

//static struct file_operations f_ops={
//	.open=uart_open,
//	.read=uart_read,
//	.write=uart_write,
//	.release=uart_close
//};
//static struct dev_object uart_obj;
//
//void uart_init(void){
//	uart_obj.f_ops=&f_ops;
//	object_reg((struct object*)&uart_obj,DEV_NAME);
//}
//
//INIT_LV2(uart_init);

