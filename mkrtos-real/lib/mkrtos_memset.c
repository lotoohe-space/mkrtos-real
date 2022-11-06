/*
 * mkrtos_memset.c
 *
 *  Created on: 2022Äê9ÔÂ4ÈÕ
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
