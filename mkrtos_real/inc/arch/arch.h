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

int32_t arch_init(void);

extern void kprint(const char *fmt, ...);
extern int int_cn_;
/**
 * @brief 关闭cpu中断
 * @return 返回之前得中断使能状态
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
	int_cn_++;
	return res;
}
/**
 * @brief 恢复cpu中断状态
 */
static inline void restore_cpu_intr(uint32_t s) {
	__asm__ __volatile__(
			"MSR     PRIMASK, %0\n"
			:
			:"r"(s)
			:
	);
	int_cn_--;

}
void set_psp(void* new_psp);
uint32_t get_psp(void);
/**
* @brief 如何在用户级进行任务切换，手动触发PendSv
*/
void to_schedule(void);
/**
 * 在用户层面执行函数，需要在psp栈上模拟值
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
* @brief 填充寄存器
* @param mem 栈内存
* @param taskFun 任务函数
* @param prg0 需要传递的参数0
* @param prg1需要传递的参数1
* @return 栈顶地址
*/
uint32_t* os_task_set_reg(uint32_t *mem_stack,
		int32_t (*task_fun)(void *arg0, void *arg1), void *prg0, void *prg1,
		void *prg2, void (*thread_exit_func)(void),void *ram);
/**
 * @brief 获取中断号
 */
uint32_t get_isr_num(void);

#endif
