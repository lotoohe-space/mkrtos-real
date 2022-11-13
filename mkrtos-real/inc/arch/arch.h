#ifndef _ARCH_H__
#define _ARCH_H__

#include "type.h"

/**
 * @brief OS����Ƶ��
 */
#define OS_WORK_HZ 1000
/**
 * @brief OS���ڵ�ms
 */
#define OS_WORK_PERIOD_MS  ((1000/OS_WORK_HZ))

int32_t arch_init(void);
/**
 * @brief �ر�cpu�ж�
 * @return ����֮ǰ���ж�ʹ��״̬
 */
static inline uint32_t dis_cpu_intr(void) {
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
/**
 * @brief �ָ�cpu�ж�״̬
 */
static inline void restore_cpu_intr(uint32_t s) {
	__asm__ __volatile__(
			"MSR     PRIMASK, %0\n"
			:
			:"r"(s)
			:
	);
}
void set_psp(void* new_psp);
uint32_t get_psp(void);
/**
* @brief ������û������������л����ֶ�����PendSv
*/
void to_schedule(void);
/**
 * ���û�����ִ�к�������Ҫ��pspջ��ģ��ֵ
 * @param callFunc
 * @param arg0
 * @param memStack
 * @return
 */
void* set_into_user_stack(void *callFunc,void *arg0,uint32_t *memStack,void* resAddr);

uint32_t *os_task_set_reg1(uint32_t *mem_stack,
		int32_t (*task_fun)(void *arg0, void *arg1), void *prg0, void *prg1,
		void *prg2, void (*thread_exit_func)(void));

/**
* @brief ���Ĵ���
* @param mem ջ�ڴ�
* @param taskFun ������
* @param prg0 ��Ҫ���ݵĲ���0
* @param prg1��Ҫ���ݵĲ���1
* @return ջ����ַ
*/
uint32_t* os_task_set_reg(uint32_t *mem_stack,
		int32_t (*task_fun)(void *arg0, void *arg1), void *prg0, void *prg1,
		void *prg2, void (*thread_exit_func)(void),void *ram);
/**
 * @brief ��ȡ�жϺ�
 */
uint32_t get_isr_num(void);

#endif
