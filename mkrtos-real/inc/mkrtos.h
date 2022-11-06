/*
 * mkrtos.h
 *
 *  Created on: 2022年7月23日
 *      Author: Administrator
 */

#ifndef INC_MKRTOS_H_
#define INC_MKRTOS_H_

#include "config.h"

/**
 * syscall调用传递的参数
 */
struct syscall_frame {
	//r0 tag 消息tag
	//r1 utcb 寄存器
	//r2 ref 调用时产生需要产生的操作
	//r3 timeout 超时
	//r4 spec spec
	int r[5];
};

#define MEM_TRACE 1 					//!<是否跟踪内存

#define MIN(a,b)	((a)<(b)?(a):(b)) 	//!< 最小值
#define MAX(a,b)	((a)>(b)?(a):(b)) 	//!< 最大值


#define SET_BIT(a,b) ((a)|=1<<(b))		//!<设置BIT

#define CLR_BIT(a,b) ((a)&=~(1<<(b)))	//!<清除BIT
#define GET_BIT(a,b) (((a)>>(b))&0x1)	//!<获取某位的bit
#define ABS(a) ((a)<0?-(a):(a))			//!<取绝对值

#define ROUND(a,b) (((a)/(b))+(((a)%(b))?1:0))	//!< a/b后的值向上取整
#define ROUND_UP(a,b) ROUND(a,b)				//!< a除b向上取整数
#define ROUND_DOWN(a,b) ((a)/(b))				//!< a/b向下取整


#define ARRARY_LEN(a) (sizeof(a)/sizeof((a)[0]))	//!<数组长度

#define ALIGN(mem,align) (((mem)+((align)-1))&(~((align)-1))) //!< 向上对齐
#define ALIGN_DOWN(mem,align) ((mem)&(~((align)-1))) //!< 向下对齐

#define container_of(ptr, type, member) ({              \
	const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
	(type *)( (char *)__mptr - offsetof(type,member) );})


static inline int constant_fls(int x)
{
	int r = 32;

	if (!x) {
		return 0;
	}
	if (!(x & 0xffff0000u)) {
		x <<= 16;
		r -= 16;
	}
	if (!(x & 0xff000000u)) {
		x <<= 8;
		r -= 8;
	}
	if (!(x & 0xf0000000u)) {
		x <<= 4;
		r -= 4;
	}
	if (!(x & 0xc0000000u)) {
		x <<= 2;
		r -= 2;
	}
	if (!(x & 0x80000000u)) {
		x <<= 1;
		r -= 1;
	}
	return r;
}
static inline int ffs(int x)
{
	int ret;
	if (__builtin_constant_p(x))
	return constant_fls(x);
	asm("clz\t%0, %1" : "=r" (ret) : "r" (x) : "cc");
	ret = 32 - ret;
	return ret;
}

static inline int is_power_of_2(int num){
	return (num & (num-1)) == 0;
}

typedef int (*init_func)(void); 		//!< 初始化函数
typedef int (*sys_call_func)(void); 	//!< 系统调用函数

#endif /* INC_MKRTOS_H_ */
