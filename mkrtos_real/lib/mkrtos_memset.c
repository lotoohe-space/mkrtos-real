/*
 * mkrtos_memset.c
 *
 *  Created on: 2022年9月4日
 *      Author: Administrator
 */
#include <type.h>

void* mkrtos_memset(void * dst, int s, size_t count) {
    register char * a = dst;
    count++;	/* this actually creates smaller code than using count-- */
    while (--count)
	*a++ = s;
    return dst;
}
void* memset(void * dst, int s, size_t count) {
    mkrtos_memset(dst, s, count);
}
