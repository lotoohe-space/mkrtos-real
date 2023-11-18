#pragma once

#define MK_SET_BIT(a, b) ((a) |= 1 << (b))                         //!< 设置BIT
#define MK_CLR_BIT(a, b) ((a) &= ~(1 << (b)))                      //!< 清除BIT
#define MK_GET_BIT(a, b) (((a) >> (b)) & 0x1)                      //!< 获取某位的bit
#define ABS(a) ((a) < 0 ? -(a) : (a))                              //!< 取绝对值
#define MIN(a, b) ((a) < (b) ? (a) : (b))                          //!< 最小值
#define ALIGN(mem, align) (((mem) + ((align)-1)) & (~((align)-1))) //!< 向上对齐
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))             //!< 获取数组大小
#define ROUND(a, b) (((a) / (b)) + (((a) % (b)) ? 1 : 0))          //!< a/b后的值向上取整
#define ROUND_UP(a, b) ROUND(a, b)                                 //!< a除b向上取整数
#define ROUND_DOWN(a, b) ((a) / (b))                               //!< a/b向下取整

#define ATTR_ALIGN(a) __attribute__((aligned(a)))
#define AUTO_CALL(prio) __attribute__((constructor(prio)))