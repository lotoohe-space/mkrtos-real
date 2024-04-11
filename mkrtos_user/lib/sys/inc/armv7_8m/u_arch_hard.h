#pragma once

#define TASK_RAM_BASE()        \
    ({                         \
        umword_t _val;         \
        __asm__ __volatile__(  \
            "mov     %0, r9\n" \
            : "=r"(_val)       \
            :                  \
            :);                \
        _val;                  \
    })

#define ARCH_REG_0 "r0"
#define ARCH_REG_1 "r1"
#define ARCH_REG_2 "r2"
#define ARCH_REG_3 "r3"
#define ARCH_REG_4 "r4"
#define ARCH_REG_5 "r5"
#define ARCH_REG_6 "r6"
#define ARCH_REG_7 "r7"

#define ARCH_WORD_SIZE 32

