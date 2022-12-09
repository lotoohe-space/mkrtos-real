/**
 * @brief 底层相关
 * @author 1358745329@qq.com
 */
#include "arch/arch.h"
#include <stm32_sys.h>
#include <arch/isr.h>
#include <mkrtos.h>
#include <config.h>

int int_cn_ = 0;

void SysTick_Handler(void);

int32_t bsp_init(void) {
	return 0;
}
INIT_LV0(bsp_init);

/**
 * 在sched.c中调用，查看sche_start函数
 */
int32_t arch_init(void) {
	extern uint32_t SystemCoreClock;
	//注册中断函数
	reg_isr_func(SysTick_Handler, 0, 0);
	SysTick_Config(SystemCoreClock / OS_WORK_HZ);
	return 0;
}

uint32_t *os_task_set_reg1(uint32_t *mem_stack,
		int32_t (*task_fun)(void *arg0, void *arg1), void *prg0, void *prg1,
		void *prg2, void (*thread_exit_func)(void))
{
	/* Registers stacked as if auto-saved on exception */
	*(mem_stack) = (uint32_t) 0x01000000L; /* xPSR */
	*(--mem_stack) = ((uint32_t) task_fun); /* Entry Point */
	/* R14 (LR) (init value will cause fault if ever used)*/
	*(--mem_stack) = (uint32_t) thread_exit_func;
	*(--mem_stack) = (uint32_t) 0x12121212L; /* R12*/
	*(--mem_stack) = (uint32_t) 0x03030303L; /* R3 */
	*(--mem_stack) = (uint32_t) prg2; /* R2 */
	*(--mem_stack) = (uint32_t) prg1; /* R1 */
	*(--mem_stack) = (uint32_t) prg0; /* R0 : argument */

	return mem_stack;
}
/**
 * @brief 填充寄存器
 * @param mem 栈内存
 * @param taskFun 任务函数
 * @param prg0 需要传递的参数0
 * @param prg1需要传递的参数1
 * @return 栈顶地址
 */
uint32_t* os_task_set_reg(uint32_t *mem_stack,
		int32_t (*task_fun)(void *arg0, void *arg1), void *prg0, void *prg1,
		void *prg2, void (*thread_exit_func)(void),void *ram) {
	extern void task_end(void);
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    u32 i;
    *(mem_stack)    = (uint32_t)0x00000000uL;
    *(--mem_stack)  = (uint32_t)0x00000000uL;

    for (i = 0; i < 16; ++i)
    {
        *(--mem_stack) = (uint32_t)(0x00000000uL);
    }

    *(--mem_stack)  = (uint32_t)0x01000000uL;      // xPSR
    *(--mem_stack)  = (uint32_t)taskFun;              // Entry Point
    *(--mem_stack)  = (uint32_t)OSTaskEnd;     // R14 (LR)
    *(--mem_stack)  = (uint32_t)0x12121212uL;      // R12
    *(--mem_stack)  = (uint32_t)0x03030303uL;      // R3
    *(--mem_stack)  = (uint32_t)0x02020202uL;      // R2
    *(--mem_stack)  = (uint32_t)prg1;      // R1
    *(--mem_stack)  = (uint32_t)prg0;             // R0 : argument

                                           // Remaining registers saved on process stack
    *(--mem_stack)  = (uint32_t)0x11111111uL;      // R11
    *(--mem_stack)  = (uint32_t)0x10101010uL;      // R10
    *(--mem_stack)  = (uint32_t)0x09090909uL;      // R9
    *(--mem_stack)  = (uint32_t)0x08080808uL;      // R8
    *(--mem_stack)  = (uint32_t)0x07070707uL;      // R7
    *(--mem_stack)  = (uint32_t)0x06060606uL;      // R6
    *(--mem_stack)  = (uint32_t)0x05050505uL;      // R5
    *(--mem_stack)  = (uint32_t)0x04040404uL;      // R4

    for (i = 0; i < 16; ++i)
    {
        *(--mem_stack) = (uint32_t)(0x00000000uL);
    }

#else
	/* Registers stacked as if auto-saved on exception */
	*(mem_stack) = (uint32_t) 0x01000000L; /* xPSR */
	*(--mem_stack) = ((uint32_t) task_fun); /* Entry Point */
	/* R14 (LR) (init value will cause fault if ever used)*/
	*(--mem_stack) = (uint32_t) thread_exit_func;
	*(--mem_stack) = (uint32_t) 0x12121212L; /* R12*/
	*(--mem_stack) = (uint32_t) 0x03030303L; /* R3 */
	*(--mem_stack) = (uint32_t) prg2; /* R2 */
	*(--mem_stack) = (uint32_t) prg1; /* R1 */
	*(--mem_stack) = (uint32_t) prg0; /* R0 : argument */
	/* Remaining registers saved on process stack */
	*(--mem_stack) = (uint32_t) 0x11111111L; /* R11 */
	*(--mem_stack) = (uint32_t) 0x10101010L; /* R10 */
	*(--mem_stack) = (uint32_t) ram; /* R9 */
	*(--mem_stack) = (uint32_t) 0x08080808L; /* R8 */
	*(--mem_stack) = (uint32_t) 0x07070707L; /* R7 */
	*(--mem_stack) = (uint32_t) 0x06060606L; /* R6 */
	*(--mem_stack) = (uint32_t) 0x05050505L; /* R5 */
	*(--mem_stack) = (uint32_t) 0x04040404L; /* R4 */
#endif

	return mem_stack;
}

//当要执行用户进程的函数时，先在栈顶放入模拟值
void* set_into_user_stack(void *callFunc, void *arg0, uint32_t *mem_stack,
		void *resAddr) {
	/* Registers stacked as if auto-saved on exception */
	*(mem_stack) = (uint32_t) 0x01000000L; /* xPSR */
	*(--mem_stack) = ((uint32_t) callFunc); /* Entry Point */
	/* R14 (LR) (init value will cause fault if ever used)*/
	*(--mem_stack) = (uint32_t) resAddr;/*LR*/
	*(--mem_stack) = (uint32_t) 0x12121212L; /* R12*/
	*(--mem_stack) = (uint32_t) 0x03030303L; /* R3 */
	*(--mem_stack) = (uint32_t) 0x02020202L; /* R2 */
	*(--mem_stack) = (uint32_t) 0x01010101L; /* R1 */
	*(--mem_stack) = (uint32_t) arg0; /* R0 : argument */
	return mem_stack;
}


void set_psp(void *new_psp) {
	__asm__ __volatile__(
			"MSR     PSP, %0\n"
			:
			:"r"(new_psp)
			:
	);
}
uint32_t get_psp(void) {
	uint32_t ret;
	__asm__ __volatile__(
			"MRS     %0, PSP\n"
			:"=r"(ret)
			:
			:
	);
	return ret;
}

/**
 * @brief 手动触发PendSv进行任务切换
 */
void to_schedule(void) {
	__asm__ __volatile__(
			"LDR R0, =0xE000ED04\n"
			"LDR R1, =0x10000000\n"
			"STR R1, [R0]\n"
			"DSB \n"
			"ISB \n"
			:
			:
			:
	);
}
//只能够在特权模式调用
//获取中断号
uint32_t get_isr_num(void) {
	uint32_t num;
	__asm__ __volatile__(
			"mrs %0,IPSR"
			:"=r"(num)
			:
			:
	);
	return num;
}
extern void tasks_check(void);
extern void task_sche(void);
//sleep.c
//extern void do_check_sleep_tim(void);
#include <ipc/knl_mutex.h>
//#include <mkrtos/knl_msg.h>
//#include <mkrtos/knl_sem.h>
void SysTick_Handler(void) {
//    check_msg_time();
//    check_sem_time();
	// check_mutex_time();
    do_check_sleep_tim();
	tasks_check();
	task_sche();
}
