//
// Created by Administrator on 2022/1/19.
//

#include <stdio.h>
#include <stdarg.h>
#include <type.h>
#include <mkrtos/fs.h>
#include <mkrtos/early_console.h>

#include "gd32f10x.h"
#include <stdio.h>
#include <xprintf.h>


static uint8_t printk_cache[512];

//static int usart_init=0;
//void usart_config(void)
//{
//    /* enable USART clock */
//    rcu_periph_clock_enable(RCU_USART0);
//
//    #if defined USART0_REMAP
//        /* enable GPIO clock */
//        rcu_periph_clock_enable(RCU_GPIOB);
//        rcu_periph_clock_enable(RCU_AF);
//        /* USART0 remapping */
//        gpio_pin_remap_config(GPIO_USART0_REMAP, ENABLE);
//        /* connect port to USARTx_Tx */
//        gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
//        /* connect port to USARTx_Rx */
//        gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
//    #else
//        /* enable GPIO clock */
//        rcu_periph_clock_enable(RCU_GPIOA);
//
//        /* connect port to USARTx_Tx */
//        gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
//        /* connect port to USARTx_Rx */
//        gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
//    #endif
//
//    /* USART configure */
//    usart_deinit(USART0);
//    usart_baudrate_set(USART0, 115200U);
//    usart_word_length_set(USART0, USART_WL_8BIT);
//    usart_stop_bit_set(USART0, USART_STB_1BIT);
//    usart_parity_config(USART0, USART_PM_NONE);
//    usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);
//    usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);
//    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
//    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
//    usart_enable(USART0);
//}

//void usart_print_str(const char *str) {
//	int i=0;
//	for(;str[i];i++) {
//		usart_data_transmit(USART0, (uint8_t)str[i]);
//		while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
//	}
//}

void ktrace(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    xvsprintf(printk_cache,fmt,args);
    va_end(args);
    console_write(printk_cache,mkrtos_strlen(printk_cache));
}
void kprint(const char *fmt, ...){
    va_list args;
    va_start(args, fmt);
    xvsprintf(printk_cache,fmt,args);
    va_end(args);
    console_write(printk_cache,mkrtos_strlen(printk_cache));
}

void kfatal(const char* fmt,...){

    va_list args;
    va_start(args, fmt);
    xvsprintf(printk_cache,fmt,args);
    va_end(args);
    console_write(printk_cache,mkrtos_strlen(printk_cache));
    while(1);
}
