#ifndef __DELAY_H
#define __DELAY_H 			   
#include "sys.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//STM32F103ZE���İ�
//ʹ��SysTick����ͨ����ģʽ���ӳٽ��й����ʺ�STM32F10xϵ�У�
//����delay_us,delay_ms

////////////////////////////////////////////////////////////////////////////////// 
	 
void delay_init(void);
void delay_ms(uint16_t nms);
void delay_us(uint32_t nus);

#endif





























