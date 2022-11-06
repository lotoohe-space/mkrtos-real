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

int32_t archinit(void);
/**
* @breif �ر�cpu�ж�
* @return ����֮ǰ���ж�ʹ��״̬
*/
uint32_t dis_cpu_intr(void);
/**
* @breif �ָ�cpu�ж�״̬
*/
void restore_cpu_intr(uint32_t s);
void set_psp(void* new_psp);
uint32_t get_psp(void);
/**
* @breif ������û������������л����ֶ�����PendSv
*/
void _task_schedule(void);
/**
 * ���û�����ִ�к�������Ҫ��pspջ��ģ��ֵ
 * @param callFunc
 * @param arg0
 * @param memStack
 * @return
 */
void* set_into_user_stack(void *callFunc,void *arg0,uint32_t *memStack,void* resAddr);
/**
* @breif ���Ĵ���
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
	);



#endif
