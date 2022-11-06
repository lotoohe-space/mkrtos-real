/*
 * mm.h
 *
 *  Created on: 2022年8月6日
 *      Author: Administrator
 */

#ifndef INC_MM_H_
#define INC_MM_H_

#include <type.h>
#include <mkrtos.h>

#define MAGIC_NUM 0xFEDCBA98
#define MEM_HEAP_NAME 4

typedef struct mem_heap {
	uint32_t magic;
	char name[MEM_HEAP_NAME];
	struct mem_heap* next;
	struct mem_heap* prev;
	uint32_t size;
	uint32_t used;
} mem_heap_t;
#define MEM_HEAP_STRUCT_SIZE ALIGN(sizeof(struct mem_heap),4) //!< 内存管理结构体大小

typedef struct mem {
	struct mem_heap* heap_start; //!< 内存开始的结构体
	struct mem_heap* heap_end; //!< 内存结束的结构体
	struct mem_heap* l_heap; //!< 最低的空闲内存的结构体
} mem_t;

void mem_init(mem_t *_this);
void mem_free(mem_t *_this, void* mem);
void* mem_alloc(mem_t *_this, uint32_t size);
int mem_heap_add(mem_t *_this, void* mem, uint32_t size);
void* mem_split(void* mem, uint32_t size);
void* mem_alloc_align(mem_t *_this, uint32_t size, uint32_t align);
void mem_free_align(mem_t *_this, void* f_mem);
struct mem_heap* mem_get_free(mem_t *_this, int32_t hope_size, uint32_t *ret_addr);

#if MEM_TRACE
void mem_trace(mem_t *_this);
#endif
#endif /* INC_MM_H_ */
