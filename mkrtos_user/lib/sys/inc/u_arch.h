#pragma once
#include "u_types.h"
#include "u_arch_hard.h"

#ifndef PAGE_SIZE
#define PAGE_SIZE (1UL << CONFIG_PAGE_SHIFT)
#endif
#define MK_PAGE_SIZE (PAGE_SIZE)
#define WORD_BYTES (sizeof(void *))
#define WORD_BITS (WORD_BYTES * 8)


static inline uint16_t swap_u16(uint16_t data)
{
    return (uint16_t)((data >> (uint16_t)8) & (uint16_t)0xffUL) |
           (uint16_t)((uint16_t)(data & (uint16_t)0xff) << (uint16_t)8);
}

int mk_syscall(unsigned long nr, ...);
