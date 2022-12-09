/*
 * exec.h
 *
 *  Created on: 2022年9月5日
 *      Author: Administrator
 */

#ifndef INC_MKRTOS_EXEC_H_
#define INC_MKRTOS_EXEC_H_

/**
 * 可执行文件信息
 */
typedef struct exec_file_info {
	const char d[32];
	const char magic[8];
	union {
		struct exec_head_info {
			uint32_t ram_size;
			uint32_t heap_offset;
			uint32_t stack_offset;
			uint32_t heap_size;
			uint32_t stack_size;
			uint32_t data_offset;
			uint32_t bss_offset;
			uint32_t got_start;
			uint32_t got_end;
			uint32_t rel_start;
			uint32_t rel_end;
			uint32_t text_start;
		} i;
		const char d1[128];
	};
	const char dot_text[];
} exec_file_info_t;

#define EXEC_MAGIC "MKRTOS. "

struct exec_file_info *exec_get(int32_t i);
int32_t exec_put(int32_t i) ;


#endif /* INC_MKRTOS_EXEC_H_ */
