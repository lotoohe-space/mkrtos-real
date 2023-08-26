#pragma once

#include "types.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b)) //!< 最小值
#define MAX(a, b) ((a) > (b) ? (a) : (b)) //!< 最大值

#define MK_SET_BIT(a, b) ((a) |= 1 << (b)) //!< 设置BIT

#define MK_CLR_BIT(a, b) ((a) &= ~(1 << (b))) //!< 清除BIT
#define MK_GET_BIT(a, b) (((a) >> (b)) & 0x1) //!< 获取某位的bit
#define ABS(a) ((a) < 0 ? -(a) : (a))         //!< 取绝对值

#define ROUND(a, b) (((a) / (b)) + (((a) % (b)) ? 1 : 0)) //!< a/b后的值向上取整
#define ROUND_UP(a, b) ROUND(a, b)                        //!< a除b向上取整数
#define ROUND_DOWN(a, b) ((a) / (b))                      //!< a/b向下取整

#define ARRARY_LEN(a) (sizeof(a) / sizeof((a)[0])) //!< 数组长度

#define ALIGN(mem, align) (((mem) + ((align)-1)) & (~((align)-1))) //!< 向上对齐
#define ALIGN_DOWN(mem, align) ((mem) & (~((align)-1)))            //!< 向下对齐
#define MASK_LSB(data, inx) (((data) >> (inx)) << (inx))           //!< 屏蔽低位

#define __ALIGN__(size) __attribute__((aligned(size)))

#define container_of(ptr, type, member) \
    ((type *)(((umword_t)(ptr)) - ((umword_t)(&(((type *)0)->member)))))

#define USED __attribute__((used))

int ffs(int x);
