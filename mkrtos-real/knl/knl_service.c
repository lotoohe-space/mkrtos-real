/*
 * knl_service.c
 *
 *  Created on: 2022Äê8ÔÂ6ÈÕ
 *      Author: Administrator
 */


#include <type.h>
#include <mm.h>
#include <mkrtos/object.h>
#include <knl_service.h>

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
struct mem_heap *knl_mem_get_free(int32_t hope_size, uint32_t *ret_addr) {
	return mem_get_free(&knl_mem, hope_size, ret_addr);
}

void *user_malloc(uint32_t size,const char* name){
	struct object *obj;
	void *mem;

	obj=object_create();
	if(!obj){
		return NULL;
	}

	mem=mem_alloc(&knl_mem, size+OBJECT_NAME_LEN);
	if(!mem){
		object_free(obj);
		return NULL;
	}
	mkrtos_strncpy((char*)mem,name,OBJECT_NAME_LEN);
	obj->user_data=mem;
	obj->type=OBJECT_MEM_TYPE;
	object_reg(&obj,name);
	return (uint8_t*)mem+OBJECT_NAME_LEN;
}
void user_free(void *mem){
	struct object *obj;
	obj=object_find((char*)mem-OBJECT_NAME_LEN);
	if(!obj){
		return ;
	}
	object_unreg(obj);
	object_free(obj);
}
