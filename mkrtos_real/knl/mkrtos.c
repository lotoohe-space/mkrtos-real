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


/**
 * @brief 第一个C函数
 * 
 */
void _start(void){
	mkrtos_init();
	
	extern int main(void);
	main();
}
