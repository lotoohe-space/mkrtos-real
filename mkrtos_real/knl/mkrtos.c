/*
 * mkrtos.c
 *
 *  Created on: 2022年7月23日
 *      Author: Administrator
 */

#include <type.h>
#include <mkrtos.h>
#include <config.h>
#include <mm.h>
#include <mkrtos/early_console.h>

extern ptr_t _mkrtos_init_start,_mkrtos_init_end;
/**
 * @brief 初始化调用
 * 
 */
static void mkrtos_init(void){
	ptr_t* func_start=(ptr_t*)(&_mkrtos_init_start);
	ptr_t* func_stop=(ptr_t*)(&_mkrtos_init_end);
	for(;func_start<func_stop;func_start++){
		((init_func)(*func_start))();
	}
}
/**
 * @brief 第一个C函数
 * 
 */
void _start(void){
	mkrtos_init();
	console_reg(0);
	extern int main(void);
	main();
}
