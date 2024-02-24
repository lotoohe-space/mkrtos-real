#pragma once
#include "u_types.h"
// #define CONFIG_SYS_SCHE_HZ 1000 //!< 系统调度频率

#define MK_PAGE_SIZE 512
#define WORD_BYTES (sizeof(void *))
#define WORD_BITS (WORD_BYTES * 8)

#define TASK_RAM_BASE()             \
    ({                         \
        umword_t _val;         \
        __asm__ __volatile__(  \
            "mov     %0, r9\n" \
            : "=r"(_val)       \
            :                  \
            :);                \
        _val;                  \
    })

static inline uint16_t swap_u16(uint16_t data)
{
    return (uint16_t)((data >> (uint16_t)8) & (uint16_t)0xffUL) |
           (uint16_t)((uint16_t)(data & (uint16_t)0xff) << (uint16_t)8);
}

int mk_syscall(unsigned long nr, ...);
