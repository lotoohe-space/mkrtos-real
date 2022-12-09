/*
 * knl_service.c
 *
 *  Created on: zhangzheng
 *      Author: Administrator
 */


#include <type.h>
#include <mm.h>
#include <mkrtos/object.h>
#include <knl_service.h>
#include <mkrtos/sched.h>

static mem_t knl_mem = {0};

static char mem_sram[41*1024];
//static char mem_ex[1024*1024] __attribute__((section(".ext_sram")));

static void mkrtos_mem_init(void){
	mem_heap_add(&knl_mem, mem_sram,sizeof(mem_sram));
//	mem_heap_add(mem_ex,sizeof(mem_ex));
}
INIT_LV1(mkrtos_mem_init);

void *malloc(uint32_t size){
	return mem_alloc(&knl_mem, size);
}
void free(void* mem){
	mem_free(&knl_mem, mem);
}
void *malloc_align(uint32_t size,uint32_t align){
	return mem_alloc_align(&knl_mem, size,align);
}
void free_align(void *mem){
	mem_free_align(&knl_mem, mem);
}
void knl_mem_trace(void){
	mem_trace(&knl_mem);
}
struct mem_heap *knl_mem_get_free(struct mem_heap *next,
	int32_t hope_size, uint32_t *ret_addr) {
	return mem_get_free(&knl_mem, next, hope_size, ret_addr);
}
/**
 * @brief 检查用户态内存是否合法
 * 
 * @param start_addr 开始地址
 * @param size 大小
 * @param op 0 读写 1 读
 * @return int 0 正确，失败
 */
int knl_check_user_mem(void *start_addr, int size, int op)
{
	task_t * curr_task = get_current_task();
	
	mword_t u_addr = TASK_USER_RAM_ADDR(curr_task);
	mword_t k_addr = TASK_USER_RAM_SIZE(curr_task);

	
}

// void *user_malloc(uint32_t size,const char* name){
// 	struct object *obj;
// 	void *mem;

// 	obj=object_create();
// 	if(!obj){
// 		return NULL;
// 	}

// 	mem=mem_alloc(&knl_mem, size+OBJECT_NAME_LEN);
// 	if(!mem){
// 		object_free(obj);
// 		return NULL;
// 	}
// 	mkrtos_strncpy((char*)mem,name,OBJECT_NAME_LEN);
// 	obj->user_data=mem;
// 	obj->type=OBJECT_MEM_TYPE;
// 	object_reg(&obj,name);
// 	return (uint8_t*)mem+OBJECT_NAME_LEN;
// }
// void user_free(void *mem){
// 	struct object *obj;
// 	obj=object_find((char*)mem-OBJECT_NAME_LEN);
// 	if(!obj){
// 		return ;
// 	}
// 	object_unreg(obj);
// 	object_free(obj);
// }
