
#include <mkrtos/sched.h>
#include "arch/isr.h"
#include "type.h"
#include "arch/arch.h"
#include "mkrtos/msg_knl.h"

//中断函数列表
static struct isr_func isr_func_list[80]={NULL};

/**
 * @berief 所有的外部中断都到这里,
 * @param 传入的是栈指针
 */
void extern_intr(void* sp){
	uint32_t isr_num;
	isr_num = get_isr_num();
    if(isr_func_list[isr_num-ISR_START_INX].isr_func!=NULL){
    	isr_func_list[isr_num-ISR_START_INX].isr_func();
    }else{
        //kprint("unreg isr %d.\r\n",isr_num);
    }
}

//注册中断函数
void reg_isr_func(isr_cb isrCb,uint32_t isr_num,uint8_t inKernel){
	uint32_t t;
	t=dis_cpu_intr();
	isr_func_list[isr_num].isr_func=isrCb;
	isr_func_list[isr_num].isr_flag|=inKernel;
	restore_cpu_intr(t);
}
//取消注册中断函数
void unreg_isr_func(uint32_t isr_num){
	uint32_t t;
	t=dis_cpu_intr();
	isr_func_list[isr_num].isr_func=NULL;
	restore_cpu_intr(t);
}


