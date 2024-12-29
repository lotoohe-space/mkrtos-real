#ifndef _NES_COMMON_H_
#define _NES_COMMON_H_
#include "u_types.h"
#include "stdlib.h"
#include <string.h>
#define LCD_WIDTH 256
#define LCD_HEIGHT 240
#define LCD_NES_XOFFSET 100
#define LCD_NES_YOFFSET 0
typedef signed char				s8_t;
typedef unsigned char			u8_t;

typedef signed short			s16_t;
typedef unsigned short			u16_t;

typedef signed int				s32_t;
typedef unsigned int			u32_t;

typedef signed long long		s64_t;
typedef unsigned long long		u64_t;

typedef signed char				s8;
typedef unsigned char			u8;

typedef signed short			s16;
typedef unsigned short			u16;

typedef signed int				s32;
typedef unsigned int			u32;

typedef signed long long		s64;
typedef unsigned long long		u64;

#define SRAMEX 0
#define SRAMIN 1

static inline void *mymalloc(int s,int size)
{
	return malloc(size);
}
static inline void myfree(int s,void * d)
{
    free(d);
}	 

static inline void mymemset(void *s,u8 c,u32 count)  
{  
    memset(s, c, count);
}
#endif