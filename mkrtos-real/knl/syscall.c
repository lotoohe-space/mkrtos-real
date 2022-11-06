/*
 * syscall.c
 *
 *  Created on: 2022��7��24��
 *      Author: Administrator
 */

#include <type.h>
#include <mkrtos.h>
#include <knl_service.h>
#include <errno.h>
#include <mkrtos/sys.h>
#include <assert.h>
#include <mkrtos/sched.h>
/**
 * �ں��ڴ�cpy���û�
 */
int knl_memcpyto_user(void* dst,int dst_len,void* src,int src_len){
	int min_len=MIN(dst_len,src_len);
	mkrtos_memcpy(dst,src,min_len);
	return 0;
}
/**
 * �û��ڴ�cpy���ں�
 */
int user_memcpyto_knl(void* dst,int dst_len,void* src,int src_len){
	int min_len=MIN(dst_len,src_len);
	mkrtos_memcpy(dst,src,min_len);
	return 0;
}

/**
 * �ɸ����̵�ַָ��ӳ�䵽�ӽ���
 */
int sys_p2c_addr(void *addr) {
	struct task *curr_task = get_current_task();

	return ((uint32_t)addr) + curr_task->parent_ram_offset;
}

int syscall_none(void){
	return -ENOSYS;
}


static const int syscall_func_number=sizeof(sys_call_table)/sizeof(sys_call_func);

void* get_syscall_handler(void* sp,int32_t call_num){
	if(call_num<0 || call_num >= syscall_func_number){
		return (void*)syscall_none;
	}else{
		MKRTOS_ASSERT(sys_call_table[call_num]);
		return sys_call_table[call_num];
	}
}
