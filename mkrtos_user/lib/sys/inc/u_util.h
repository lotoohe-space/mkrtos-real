#pragma once

#define MK_SET_BIT(a, b) ((a) |= 1 << (b))                         //!< 设置BIT
#define MK_CLR_BIT(a, b) ((a) &= ~(1 << (b)))                      //!< 清除BIT
#define MK_GET_BIT(a, b) (((a) >> (b)) & 0x1)                      //!< 获取某位的bit
#define ABS(a) ((a) < 0 ? -(a) : (a))                              //!< 取绝对值
#define MIN(a, b) ((a) < (b) ? (a) : (b))                          //!< 最小值
#define ALIGN(mem, align) (((mem) + ((align)-1)) & (~((align)-1))) //!< 向上对齐
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))             //!< 获取数组大小