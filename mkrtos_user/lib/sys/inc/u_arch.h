#pragma once

#define WORD_BYTES (sizeof(void *))
#define WORD_BITS (WORD_BYTES * 8)

#define RAM_BASE()             \
    ({                         \
        umword_t _val;         \
        __asm__ __volatile__(  \
            "mov     %0, r9\n" \
            : "=r"(_val)       \
            :                  \
            :);                \
        _val;                  \
    })
