#ifndef _ARCH_H__
#define _ARCH_H__

#include "type.h"

/**
 * @brief OS工作频率
 */
#define OS_WORK_HZ 1000
/**
 * @brief OS周期的ms
 */
#define OS_WORK_PERIOD_MS  ((1000/OS_WORK_HZ))

int32_t archinit(void);
/**
* @breif 关闭cpu中断
* @return 返回之前得中断使能状态
*/
uint32_t dis_cpu_intr(void);
/**
* @breif 恢复cpu中断状态
*/
void restore_cpu_intr(uint32_t s);
void set_psp(void* new_psp);
uint32_t get_psp(void);
/**
* @breif 如何在用户级进行任务切换，手动触发PendSv
*/
void _task_schedule(void);
/**
 * 在用户层面执行函数，需要在psp栈上模拟值
 * @param callFunc
 * @param arg0
 * @param memStack
 * @return
 */
void* set_into_user_stack(void *callFunc,void *arg0,uint32_t *memStack,void* resAddr);
/**
* @breif 填充寄存器
* @param mem 栈内存
* @param taskFun 任务函数
* @param prg0 需要传递的参数0
* @param prg1需要传递的参数1
* @return 栈顶地址
*/
uint32_t* os_task_set_reg(
		uint32_t * memStack,
		void (*taskFun)(void*arg0,void *arg1),
		void *prg0,
		void *prg1,
        void *prg2
	);



#endif
