/*
 * uart.c
 *
 *  Created on: 2022年8月6日
 *      Author: Administrator
 */

#include <mkrtos/tty.h>
#include <config.h>
#include <arch/isr.h>
#include "gd32f10x.h"

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
	usart_deinit(USART0);
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
			usart_data_transmit(USART0, (uint8_t) r);
			while (RESET == usart_flag_get(USART0, USART_FLAG_TBE))
				;
		}
		w_len++;
	}
	return w_len;
}
static void uart_set_par(uint32_t usart_periph, int32_t csize, int32_t cstopb,
		int32_t par) {
	uint32_t w_tmp = 0;
	switch (csize) {
	case CS5:
		usart_word_length_set(usart_periph, 5);
		break;
	case CS6:
		usart_word_length_set(usart_periph, 6);
		break;
	case CS7:
		usart_word_length_set(usart_periph, 7);
		break;
	case CS8:
		usart_word_length_set(usart_periph, 8);
		break;
	}

	switch (cstopb) {
	case 0:
		usart_stop_bit_set(usart_periph, 1);
		break;
	case 1:
		usart_stop_bit_set(usart_periph, 2);
		break;
	}
	switch (par) {
	case 0:
		usart_parity_config(usart_periph, USART_PM_NONE);
		break;
	case 1:
		usart_parity_config(usart_periph, USART_PM_ODD);
		break;
	case 2:
		usart_parity_config(usart_periph, USART_PM_EVEN);
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
				usart_baudrate_set(USART0, 2400);
				break;
			case B4800:
				usart_baudrate_set(USART0, 4800);
				break;
			case B9600:
				usart_baudrate_set(USART0, 9600);
				break;
			case B19200:
				usart_baudrate_set(USART0, 19200);
				break;
			case B38400:
				usart_baudrate_set(USART0, 38400);
				break;
			case B57600:
				usart_baudrate_set(USART0, 57600);
				break;
		    case B115200:
				usart_baudrate_set(USART0, 115200);
				break;
			case B230400:
				usart_baudrate_set(USART0, 230400);
				break;
			case B460800:
				usart_baudrate_set(USART0, 460800);
				break;
			case B500000:
				usart_baudrate_set(USART0, 500000);
				break;
			case B576000:
				usart_baudrate_set(USART0, 576000);
				break;
			case B921600:
				usart_baudrate_set(USART0, 921600);
				break;
			case B1000000:
				usart_baudrate_set(USART0, 1000000);
				break;
			case B1152000:
				usart_baudrate_set(USART0, 1152000);
				break;
			case B1500000:
				usart_baudrate_set(USART0, 1500000);
				break;
			case B2000000:
				usart_baudrate_set(USART0, 2000000);
				break;
			case B2500000:
				usart_baudrate_set(USART0, 2500000);
				break;
			case B3000000:
				usart_baudrate_set(USART0, 3000000);
				break;
			case B3500000:
				usart_baudrate_set(USART0, 3500000);
				break;
			case B4000000:
				usart_baudrate_set(USART0, 4000000);
				break;
			}
		}
		uart_set_par(USART0, F_C_CSIZE(s_s->c_cflag), F_C_CSTOPB(s_s->c_cflag) ? 1 : 0,
				F_C_PARENB(s_s->c_cflag) ? 1 : 0);
		usart_hardware_flow_rts_config(USART0,
				F_C_CRTSCTS(s_s->c_cflag) ? USART_RTS_ENABLE : USART_RTS_DISABLE);
		usart_hardware_flow_cts_config(USART0,
				F_C_CRTSCTS(s_s->c_cflag) ? USART_RTS_ENABLE : USART_RTS_DISABLE);
		curr_ss.c_cflag = s_s->c_cflag;
		break;
	case TTY_IO_GET:
		s_s->c_cflag = curr_ss.c_cflag;
		break;

	}
	return 0;
}
extern void wake_dont_lock(struct wait_queue *queue);
static void USART0_IRQHandler(void)
{
	struct tty_queue *w_tmp;
	w_tmp = &curr_tty->r_queue;
    if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE)){
        /* receive data */
        uint8_t data = usart_data_receive(USART0);
        q_add(w_tmp, data);
        wake_up(curr_tty->r_wait);
    }
}
int32_t uart_open(struct tty_struct *tty, struct file *filp) {

	if (init_flag) {
		return 0;
	}
	/* enable USART clock */
	rcu_periph_clock_enable(RCU_USART0);

#if defined USART0_REMAP
		/* enable GPIO clock */
		rcu_periph_clock_enable(RCU_GPIOB);
		rcu_periph_clock_enable(RCU_AF);
		/* USART0 remapping */
		gpio_pin_remap_config(GPIO_USART0_REMAP, ENABLE);
		/* connect port to USARTx_Tx */
		gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
		/* connect port to USARTx_Rx */
		gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
	#else
	/* enable GPIO clock */
	rcu_periph_clock_enable(RCU_GPIOA);

	/* connect port to USARTx_Tx */
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
	/* connect port to USARTx_Rx */
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
#endif
	nvic_irq_enable(USART0_IRQn, 0, 0);
	/* USART configure */
	usart_deinit(USART0);
	usart_baudrate_set(USART0, 115200U);
	usart_word_length_set(USART0, USART_WL_8BIT);
	usart_stop_bit_set(USART0, USART_STB_1BIT);
	usart_parity_config(USART0, USART_PM_NONE);
	usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);
	usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);
	usart_receive_config(USART0, USART_RECEIVE_ENABLE);
	usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
	usart_enable(USART0);
	usart_interrupt_enable(USART0, USART_INT_RBNE);

	reg_isr_func(USART0_IRQHandler, USART0_IRQn + 1, 0);
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

