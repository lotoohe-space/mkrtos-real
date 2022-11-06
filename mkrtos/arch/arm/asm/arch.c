/**
* @brief �ײ����
* @author 1358745329@qq.com
*/
#include "arch/arch.h"
#include "sys.h"
#include "arch/isr.h"
void SysTick_Handler(void);
extern void FSMC_SRAM_Init(void);
extern int sram_test(void);
extern uint8_t RTC_Init(void);
//tim3.c
extern void TIM3_BASEInitSys(int16_t arr,uint16_t psc);
int32_t bsp_init(void){
    FSMC_SRAM_Init();
    if(sram_test()<0){
//        fatalk("�ڴ��ʼ��ʧ��!\r\n");
    }
    RTC_Init();
    TIM3_BASEInitSys(719,99);
    return 0;
}
int32_t arch_init(void){

    //ע���жϺ���
    RegIsrFunc(SysTick_Handler,0,0);
    SysTick_Config(72000000 / OS_WORK_HZ);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    return 0;
}

//__global_reg(6) zhangzheng
uint32_t  r9;//r9�Ĵ���
/**
* @brief ���Ĵ���
* @param mem ջ�ڴ�
* @param taskFun ������
* @param prg0 ��Ҫ���ݵĲ���0
* @param prg1��Ҫ���ݵĲ���1
* @return ջ����ַ
*/
uint32_t* os_task_set_reg(
		uint32_t * memStack,
		void (*taskFun)(void*arg0,void *arg1),
		void *prg0,
		void *prg1,
        void *prg2
	){
    void TaskToEnd(int32_t exitCode);
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    u32 i;
    *(memStack)    = (uint32_t)0x00000000uL;
    *(--memStack)  = (uint32_t)0x00000000uL;

    for (i = 0; i < 16; ++i)
    {
        *(--memStack) = (uint32_t)(0x00000000uL);
    }

    *(--memStack)  = (uint32_t)0x01000000uL;      // xPSR
    *(--memStack)  = (uint32_t)taskFun;              // Entry Point
    *(--memStack)  = (uint32_t)OSTaskEnd;     // R14 (LR)
    *(--memStack)  = (uint32_t)0x12121212uL;      // R12
    *(--memStack)  = (uint32_t)0x03030303uL;      // R3
    *(--memStack)  = (uint32_t)0x02020202uL;      // R2
    *(--memStack)  = (uint32_t)prg1;      // R1
    *(--memStack)  = (uint32_t)prg0;             // R0 : argument

                                           // Remaining registers saved on process stack
    *(--memStack)  = (uint32_t)0x11111111uL;      // R11
    *(--memStack)  = (uint32_t)0x10101010uL;      // R10
    *(--memStack)  = (uint32_t)0x09090909uL;      // R9
    *(--memStack)  = (uint32_t)0x08080808uL;      // R8
    *(--memStack)  = (uint32_t)0x07070707uL;      // R7
    *(--memStack)  = (uint32_t)0x06060606uL;      // R6
    *(--memStack)  = (uint32_t)0x05050505uL;      // R5
    *(--memStack)  = (uint32_t)0x04040404uL;      // R4

    for (i = 0; i < 16; ++i)
    {
        *(--memStack) = (uint32_t)(0x00000000uL);
    }

#else
	/* Registers stacked as if auto-saved on exception */
	*(memStack) = (uint32_t)0x01000000L; /* xPSR */
	*(--memStack) = ((uint32_t)taskFun); /* Entry Point */
	/* R14 (LR) (init value will cause fault if ever used)*/
	*(--memStack) = (uint32_t)TaskToEnd;
	*(--memStack) = (uint32_t)0x12121212L; /* R12*/
	*(--memStack) = (uint32_t)0x03030303L; /* R3 */
	*(--memStack) = (uint32_t)prg2; /* R2 */
	*(--memStack) = (uint32_t)prg1; 				/* R1 */
	*(--memStack) = (uint32_t)prg0; 				/* R0 : argument */
	/* Remaining registers saved on process stack */
	*(--memStack) = (uint32_t)0x11111111L; /* R11 */
	*(--memStack) = (uint32_t)0x10101010L; /* R10 */
	*(--memStack) = (uint32_t)r9; /* R9 */
	*(--memStack) = (uint32_t)0x08080808L; /* R8 */
	*(--memStack) = (uint32_t)0x07070707L; /* R7 */
	*(--memStack) = (uint32_t)0x06060606L; /* R6 */
	*(--memStack) = (uint32_t)0x05050505L; /* R5 */
	*(--memStack) = (uint32_t)0x04040404L; /* R4 */	
#endif

	return memStack;
}

//��Ҫִ���û����̵ĺ���ʱ������ջ������ģ��ֵ
void* set_into_user_stack(void *callFunc,void *arg0,uint32_t *memStack,void* resAddr){
		/* Registers stacked as if auto-saved on exception */
	*(memStack) = (uint32_t)0x01000000L; /* xPSR */
	*(--memStack) = ((uint32_t)callFunc); /* Entry Point */
	/* R14 (LR) (init value will cause fault if ever used)*/
	*(--memStack) = (uint32_t)resAddr;/*LR*/
	*(--memStack) = (uint32_t)0x12121212L; /* R12*/
	*(--memStack) = (uint32_t)0x03030303L; /* R3 */
	*(--memStack) = (uint32_t)0x02020202L; /* R2 */
	*(--memStack) = (uint32_t)0x01010101L; 				/* R1 */
	*(--memStack) = (uint32_t)arg0; 				/* R0 : argument */
	return memStack;
}

/**
* @brief �ر�cpu�ж�
* @return ����֮ǰ���ж�ʹ��״̬
*/
uint32_t dis_cpu_intr(void){
    uint32_t res;
    __asm__ __volatile__(
    "MRS     %0, PRIMASK\n"
    "CPSID   I\n"
    :"=r"(res)
    :
    :
    );
    return res;
}
void set_psp(void*  new_psp){
    __asm__ __volatile__(
    "MSR     PSP, %0\n"
    :
    :"r"(new_psp)
    :
    );
}
uint32_t get_psp(void){
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
* @brief �ָ�cpu�ж�״̬
*/
void restore_cpu_intr(uint32_t s){
    __asm__ __volatile__(
    "MSR     PRIMASK, %0\n"
    :
    :"r"(s)
    :
    );
}


/**
* @brief �ֶ�����PendSv���������л�
*/
void _task_schedule(void){
  //  uint32_t t;
   // t=DisCpuInter();
    __asm__ __volatile__(
    "LDR R0, =0xE000ED04\n"
    "LDR R1, =0x10000000\n"
    "STR R1, [R0]\n"
    :
    :
    :
    );
  //  RestoreCpuInter(t);
}

extern void tasks_check(void);
extern void task_sche(void);
//sleep.c
extern void do_check_sleep_tim(void);
#include <mkrtos/knl_mutex.h>
#include <mkrtos/knl_msg.h>
#include <mkrtos/knl_sem.h>
void SysTick_Handler(void){
    check_msg_time();
    check_sem_time();
    check_mutex_time();
    do_check_sleep_tim();
    tasks_check();
    task_sche();
}
