/*
 * slot.c
 *
 *  Created on: 2022Äê7ÔÂ26ÈÕ
 *      Author: Administrator
 */



#include <mkrtos/sched.h>
#include <ipc/spin_lock.h>
#include <mkrtos/object.h>
#include <assert.h>
#include <ipc/slot.h>


int slot_reg(struct slot_object* slot,const char* name){
	slot->parent.type=OBJECT_SLOT_TYPE;
	object_reg((struct object*)slot,name);
	return 0;
}
int slot_kinvoke(struct syscall_frame *f)
{

	return -1;
}
int slot_call(const char *name,struct slot_args *arg,int *ret){
	struct slot_object *obj;

	obj = (struct slot_object*)object_find(name);
	if(!obj){
		return -1;
	}
	MKRTOS_ASSERT(obj->parent.type == OBJECT_SLOT_TYPE);

	if(obj->func_isr && obj->src_task){
		*ret=obj->func_isr(arg->args[0],arg->args[1],arg->args[2],arg->args[3]);
	}

	return 0;
}





