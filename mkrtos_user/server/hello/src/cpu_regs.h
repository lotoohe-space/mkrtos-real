#pragma once

#include "u_types.h"
typedef struct cpu_regs
{
    struct
    {
        umword_t regs[31]; //!< 基础寄存器
        umword_t sp;       //!< sp
        umword_t pc;       //!< pc
        umword_t pstate;   //!< pstate
    };

    umword_t orig_x0;
    uint32_t syscallno;
    uint32_t unused2;

    umword_t orig_addr_limit;
    umword_t unused;
    umword_t stackframe[2];
} cpu_regs_t;
