/*
 * mkrtos.h
 *
 *  Created on: 2022��7��23��
 *      Author: Administrator
 */

#ifndef INC_MKRTOS_H_
#define INC_MKRTOS_H_

#include "config.h"

/**
 * syscall���ô��ݵĲ���
 */
struct syscall_frame {
	//r0 tag ��Ϣtag
	//r1 utcb �Ĵ���
	//r2 ref ����ʱ������Ҫ�����Ĳ���
	//r3 timeout ��ʱ
	//r4 spec spec
	int r[5];
};

#define MEM_TRACE 1 					//!<�Ƿ�����ڴ�

#define MIN(a,b)	((a)<(b)?(a):(b)) 	//!< ��Сֵ
#define MAX(a,b)	((a)>(b)?(a):(b)) 	//!< ���ֵ


#define SET_BIT(a,b) ((a)|=1<<(b))		//!<����BIT

#define CLR_BIT(a,b) ((a)&=~(1<<(b)))	//!<���BIT
#define GET_BIT(a,b) (((a)>>(b))&0x1)	//!<��ȡĳλ��bit
#define ABS(a) ((a)<0?-(a):(a))			//!<ȡ����ֵ

#define ROUND(a,b) (((a)/(b))+(((a)%(b))?1:0))	//!< a/b���ֵ����ȡ��
#define ROUND_UP(a,b) ROUND(a,b)				//!< a��b����ȡ����
#define ROUND_DOWN(a,b) ((a)/(b))				//!< a/b����ȡ��


#define ARRARY_LEN(a) (sizeof(a)/sizeof((a)[0]))	//!<���鳤��

#define ALIGN(mem,align) (((mem)+((align)-1))&(~((align)-1))) //!< ���϶���
#define ALIGN_DOWN(mem,align) ((mem)&(~((align)-1))) //!< ���¶���

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

typedef int (*init_func)(void); 		//!< ��ʼ������
typedef int (*sys_call_func)(void); 	//!< ϵͳ���ú���

#endif /* INC_MKRTOS_H_ */
