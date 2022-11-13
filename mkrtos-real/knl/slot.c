/*
 * slot.c
 *
 *  Created on: 2022年7月26日
 *      Author: Administrator
 */

#include <mkrtos/sched.h>
#include <ipc/spin_lock.h>
#include <mkrtos/object.h>
#include <assert.h>
#include <ipc/slot.h>
#include <mm.h>

typedef struct cm3_regs{
	uint32_t regs[8];
	uint32_t regs_ext[8];
} cm3_regs_t;

typedef int (*func_call)(int arg0,int arg1,int arg2);
typedef void (*func_exit)(void *user_psp);
#define SLOT_NAME_LEN 16
typedef struct slot{
	struct task *func_task;
	func_call f_call;
	func_exit f_exit;
	char name[SLOT_NAME_LEN];
	uint8_t used;
} slot_t;

#define SLOT_NUMBER 3
static slot_t slot_list[SLOT_NUMBER] = {0};

int32_t slot_reg(char* func_name, func_call fc, func_exit exit_func)
{
	struct task *curr_task = get_current_task();

	if (!fc) {
		return -EINVAL;
	}
	for(int i=0;i< SLOT_NUMBER;i++) {
		if (slot_list[i].used == 0) {
			slot_list[i].used = 1;
			slot_list[i].f_call = fc;
			slot_list[i].f_exit = exit_func;
			slot_list[i].func_task = curr_task;
			mkrtos_strcpy(slot_list[i].name, func_name);
			return 0;
		}
	}
	return -ENOENT;
}
int32_t slot_get(char* func_name)
{
	for(int i=0;i< SLOT_NUMBER;i++) {
		if (slot_list[i].used &&
				mkrtos_strcmp(func_name, slot_list[i].name)) {
			return i;
		}
	}
	return -ENOENT;
}
int32_t slot_exit_call(void)
{
	struct task *curr_task = get_current_task();

	cm3_regs_t *regs =  (cm3_regs_t *)curr_task->saving_user_stack;
	int slot_id = regs->regs[0];
	if (slot_id<0 || slot_id>=SLOT_NUMBER) {
		return -EINVAL;
	}
	if (slot_list[slot_id].used == 0) {
		return -ENOENT;
	}
	struct task *targ_task = slot_list[slot_id].func_task;

	mem_free_align(&targ_task->user_head_alloc,curr_task->helper_stack);
	curr_task->helper_stack = NULL;
	clear_helper();
	set_psp(curr_task->saving_user_stack);
	curr_task->saving_user_stack = NULL;
	mpu_update_to(curr_task);
	return 0;
}
//栈的处理
//用客户端的线程去调用服务端的代码
//吧客户端的mpu，text，和ram设置为服务端的，去运行服务端的代码。
//内核栈可以直接使用客户端的，但是用户栈需要从服务端分配内存。
//在执行完成代码退出后，需要还原客户端的mpu,text,ram。
//此时假设有多个应用去调用服务端的代码，只是增加了服务端mpu,text和ram的引用计数，其它不变。
//每次ipc的调用都需要存放一些调用的临时信息，吧这些临时信息存放当前的task中，因为当前task是一个独立的个体。
int32_t slot_call(void *user_stack)
{
	cm3_regs_t *regs = (cm3_regs_t *)user_stack;
	int slot_id = regs->regs[0];

	if (slot_id<0 || slot_id>=SLOT_NUMBER) {
		return -EINVAL;
	}
	struct task *curr_task = get_current_task();

	if (slot_list[slot_id].used == 0) {
		return -ENOENT;
	}
	struct task *targ_task = slot_list[slot_id].func_task;

	void *new_stack = mem_alloc_align(&targ_task->user_head_alloc, 512, 8);

	if (!new_stack) {
		return -ENOMEM;
	}
	//更新mpu
	mpu_update_to(targ_task);

	uint32_t *stack_top = (uint32_t*)((uint64_t)new_stack + 512 - 4);

	stack_top = os_task_set_reg1(stack_top,
			(uint32_t)(slot_list[slot_id].f_call) | 0x1,
			regs->regs[1], regs->regs[2], regs->regs[3],
			slot_list[slot_id].f_exit);
	set_helper(targ_task);
	curr_task->helper_stack = new_stack;
	//TODO:可以考虑吧所有要保存的临时信息都放到当前任务的用户栈上。
	curr_task->saving_user_stack = user_stack;
	__asm__ __volatile__(
				"mov     r9, %0\n"
				:
				:"r"(targ_task->user_ram)
				:
		);
	//设置返回的用户栈
	set_psp(stack_top);
	return 0;
}
int32_t slot_unreg(func_call fc)
{
	for(int i=0;i< SLOT_NUMBER;i++) {
		if (slot_list[i].used && slot_list[i].func_task == fc) {
			slot_list[i].used = 0;
			slot_list[i].f_call = NULL;
			slot_list[i].func_task = NULL;
			return 0;
		}
	}
}

