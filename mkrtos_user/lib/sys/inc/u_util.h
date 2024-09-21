#pragma once

#define MK_SET_BIT(a, b) ((a) |= 1 << (b))                         //!< 设置BIT
#define MK_CLR_BIT(a, b) ((a) &= ~(1 << (b)))                      //!< 清除BIT
#define MK_GET_BIT(a, b) (((a) >> (b)) & 0x1)                      //!< 获取某位的bit
#define ABS(a) ((a) < 0 ? -(a) : (a))                              //!< 取绝对值
#define MIN(a, b) ((a) < (b) ? (a) : (b))                          //!< 最小值
#define ALIGN(mem, align) (((mem) + ((align)-1)) & (~((align)-1))) //!< 向上对齐
#define ALIGN_DOWN(mem, align) ((mem) & (~((align)-1))) //!< 向上对齐
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))             //!< 获取数组大小
#define ROUND(a, b) (((a) / (b)) + (((a) % (b)) ? 1 : 0))          //!< a/b后的值向上取整
#define ROUND_UP(a, b) ROUND(a, b)                                 //!< a除b向上取整数
#define ROUND_DOWN(a, b) ((a) / (b))                               //!< a/b向下取整

#define USED __attribute__((used))
#define UNUSED __attribute__((unused))
#define PACKED __attribute__((packed))
#define SECTION(section) __attribute__((__section__(section)))
#define __ALIGN__(size) __attribute__((aligned(size)))

#define __WEAK__ __attribute__((weak))

#define container_of(ptr, type, member) \
    ((type *)(((umword_t)(ptr)) - ((umword_t)(&(((type *)0)->member)))))

#define ATTR_ALIGN(a) __attribute__((aligned(a)))
#define AUTO_CALL(prio) __attribute__((constructor(prio)))

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

/* Import a binary file */
#define IMPORT_BIN(sect, file, sym) asm (\
    ".section " #sect "\n"                  /* Change section */\
    ".balign 4\n"                           /* Word alignment */\
    ".global " #sym "\n"                    /* Export the object address */\
    #sym ":\n"                              /* Define the object label */\
    ".incbin \"" file "\"\n"                /* Import the file */\
    ".global _sizeof_" #sym "\n"            /* Export the object size */\
    ".set _sizeof_" #sym ", . - " #sym "\n" /* Define the object size */\
    ".balign 4\n"                           /* Word alignment */\
    ".section \".text\"\n")                 /* Restore section */
