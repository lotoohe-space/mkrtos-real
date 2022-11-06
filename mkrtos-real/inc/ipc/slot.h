/*
 * slot.h
 *
 *  Created on: 2022Äê7ÔÂ26ÈÕ
 *      Author: Administrator
 */

#ifndef INC_IPC_SLOT_H_
#define INC_IPC_SLOT_H_

#include <mkrtos/object.h>


#define SLOT_ARGS_NUM 4
struct slot_args{
	ptr_t args[SLOT_ARGS_NUM];
};

struct slot_object{
	struct object parent;

	struct task* src_task;
	int (*func_isr)(int arg0,int arg1,int arg2,int arg3);
};

int slot_reg(struct slot_object* slot,const char* name);
int slot_call(const char *name,struct slot_args *arg,int *ret);

#endif /* INC_IPC_SLOT_H_ */
