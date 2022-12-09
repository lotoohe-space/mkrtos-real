/*
 * mkrtos_memcpy.c
 *
 *  Created on: 2022��7��30��
 *      Author: Administrator
 */


#include <type.h>

void mkrtos_memcpy(void *dst,void *src,int len){
	register char* b_dst=dst;
	register char* b_src=src;
	for(;len;len--){
		(*b_dst++)=(*b_src++);
	}
}

