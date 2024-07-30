#pragma once

#include "types.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b)) //!< 最小值
#define MAX(a, b) ((a) > (b) ? (a) : (b)) //!< 最大值

#define MK_SET_BIT(a, b) ((a) |= 1UL << (b))    //!< 设置BIT
#define MK_CLR_BIT(a, b) ((a) &= ~(1UL << (b))) //!< 清除BIT
#define MK_GET_BIT(a, b) (((a) >> (b)) & 0x1) //!< 获取某位的bit
#define ABS(a) ((a) < 0 ? -(a) : (a))         //!< 取绝对值

#define ROUND(a, b) (((a) / (b)) + (((a) % (b)) ? 1 : 0)) //!< a/b后的值向上取整
#define ROUND_UP(a, b) ROUND(a, b)                        //!< a除b向上取整数
#define ROUND_DOWN(a, b) ((a) / (b))                      //!< a/b向下取整

#define GET_LSB(a, b) (a & (~((~0ULL) << (b))))
#define MASK_LSB(a, b) ((a >> (b)) << (b)) //!< 屏蔽低位

#define ARRARY_LEN(a) (sizeof(a) / sizeof((a)[0])) //!< 数组长度

#define ALIGN(mem, align) (((mem) + ((align) - 1)) & (~((align) - 1))) //!< 向上对齐
#define ALIGN_DOWN(mem, align) ((mem) & (~((align) - 1)))              //!< 向下对齐

#define PACKED __attribute__((packed))
#define SECTION(section) __attribute__((__section__(section)))
#define __ALIGN__(size) __attribute__((aligned(size)))

#define __WEAK__ __attribute__((weak))

#define container_of(ptr, type, member) \
    ((type *)(((umword_t)(ptr)) - ((umword_t)(&(((type *)0)->member)))))


#define USED __attribute__((used))
#define UNUSED __attribute__((unused))

#define __ARG_PLACEHOLDER_1 0,
#define __take_second_arg(__ignored, val, ...) val
#define __is_defined(x) ___is_defined(x)
#define ___is_defined(val) ____is_defined(__ARG_PLACEHOLDER_##val)
#define ____is_defined(arg1_or_junk) __take_second_arg(arg1_or_junk 1, 0)
#define __or(x, y) ___or(x, y)
#define ___or(x, y) ____or(__ARG_PLACEHOLDER_##x, y)
#define ____or(arg1_or_junk, y) __take_second_arg(arg1_or_junk 1, y)
#define IS_BUILTIN(option) __is_defined(option)
#define IS_MODULE(option) __is_defined(option##_MODULE)

#define IS_ENABLED(option) __or(IS_BUILTIN(option), IS_MODULE(option))

int ffs(int x);
int clz(umword_t x);

static inline umword_t is_power_of_2(umword_t num)
{
    return (num & (num - 1)) == 0;
}
