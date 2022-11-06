#ifndef _ISR_H__
#define _ISR_H__

#include "type.h"

#define ISR_START_INX 15
//中断从第16开始，前面是系统的异常
typedef void (*isr_cb)(void);
struct isr_func{
	//中断的回调函数
	isr_cb isr_func;
	//0bit:isr在属于内核还是用户
	uint8_t isr_flag;
};

void extern_intr(void* sp);


//注册中断函数
void reg_isr_func(isr_cb isrCb,uint32_t isrNum,uint8_t inKernel);
//取消注册中断函数
void unreg_isr_func(uint32_t isrNum);

#endif
