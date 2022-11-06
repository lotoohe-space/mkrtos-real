#include "delay.h"

extern void SysCtlDelay(uint32_t ulCount);

void delay_us(uint32_t nus)
{		
	SysCtlDelay(12*nus);
}

void delay_ms(uint16_t nms)
{	 		  
    delay_us(1000*nms);
} 









































