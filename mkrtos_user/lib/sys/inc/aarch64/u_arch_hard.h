#pragma once

#define TASK_RAM_BASE()    \
    ({                     \
        umword_t _val = 0; \
        _val;              \
    })

#define ARCH_REG_0 "x0"
#define ARCH_REG_1 "x1"
#define ARCH_REG_2 "x2"
#define ARCH_REG_3 "x3"
#define ARCH_REG_4 "x4"
#define ARCH_REG_5 "x5"
#define ARCH_REG_6 "x6"
#define ARCH_REG_7 "x7"

#define ARCH_WORD_SIZE 64

