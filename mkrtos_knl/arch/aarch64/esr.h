#pragma once
#include "types.h"
static inline mword_t esr_get(void)
{
    mword_t esr;

    asm volatile("mrs %0, ESR_EL2" : "=r"(esr));
    return esr;
}

#define arm_esr_ec(esr) ((esr) >> 26)

