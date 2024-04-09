/**
 * @file arch.h
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-10-04
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include "types.h"
#include <aarch64_ptregs.h>
#include <asm/mm.h>
#include <mmu.h>
#include <thread_arch.h>

#define ARCH_WORD_SIZE 64
#define SYSTICK_INTR_NO 30
#define LOG_INTR_NO    37 // USART1_IRQn

/// @brief 线程信息
typedef struct
{
    umword_t rg0[8]; //!< r0-r3
    umword_t r12;
    umword_t lr;
    umword_t pc;
} pf_s_t;
typedef struct pf {
    struct
    {
        mword_t regs[31]; //!< 基础寄存器
        mword_t sp;       //!< sp
        mword_t pc;       //!< pc
        mword_t pstate;   //!< pstate
    };

    mword_t orig_x0;
    uint32_t syscallno;
    uint32_t unused2;

    mword_t orig_addr_limit;
    mword_t unused;
    mword_t stackframe[2];
} pf_t;

typedef struct sp_info {
    mword_t x19;
    mword_t x20;
    mword_t x21;
    mword_t x22;
    mword_t x23;
    mword_t x24;
    mword_t x25;
    mword_t x26;
    mword_t x27;
    mword_t x28;
    mword_t fp; // x29
    mword_t sp;
    mword_t pc;
    mword_t u_sp; // user_sp
} sp_info_t;

#define _barrier() __asm__ __volatile__("" : : : "memory")
#define _dmb(ins) \
    asm volatile("dmb " #ins : : : "memory")
#define _isb() asm volatile("isb" : : : "memory")
#define _dsb(ins) \
    asm volatile("dsb " #ins : : : "memory")

#define __arch_getl(a)    (*(volatile unsigned int *)(a))
#define __arch_putl(v, a) (*(volatile unsigned int *)(a) = (v))

#define __iormb() _barrier()
#define __iowmb() _barrier()

#define readl(c)     ({ unsigned int  __v = __arch_getl(c); __iormb(); __v; })
#define writel(v, c) ({ unsigned int  __v = v; __iowmb(); __arch_putl(__v,c); })

#define read_reg(addr) (*((volatile umword_t *)(addr)))
#define write_reg(addr, data)                    \
    do {                                         \
        _barrier();                              \
        (*((volatile umword_t *)(addr))) = data; \
        _barrier();                              \
    } while (0)

#define read_reg32(addr) (*((volatile uint32_t *)(addr)))
#define write_reg32(addr, data)                            \
    do {                                                   \
        _barrier();                                        \
        (*((volatile uint32_t *)(addr))) = (uint32_t)data; \
        _barrier();                                        \
    } while (0)

//! 读取系统寄存器
#define read_sysreg(reg) ({      \
    unsigned long _val;          \
    asm volatile("mrs %0, " #reg \
                 : "=r"(_val));  \
    _val;                        \
})
//! 写入系统寄存器
#define write_sysreg(val, reg) ({                   \
    unsigned long _val = (unsigned long)val;        \
    asm volatile("msr " #reg ", %x0" ::"rZ"(_val)); \
})

static inline uint64_t arch_get_currentel(void)
{
    unsigned long _val;

    asm volatile("mrs %0, CurrentEL"
                 : "=r"(_val));
    return _val;
}
static inline int arch_get_current_cpu_id(void)
{
    return read_sysreg(mpidr_el1) & 0XFFUL;
}

#define is_run_knl() \
    ({/*TODO:*/      \
      0})

void to_sche(void);

#define get_sp() (                                 \
    {                                              \
        mword_t sp;                                \
        do {                                       \
            asm volatile("mov %0, sp" : "=r"(sp)); \
        } while (0);                               \
        sp;                                        \
    })
#define set_sp(sp) (                                \
    {                                               \
        do {                                        \
            asm volatile("mov sp, %0" : : "r"(sp)); \
        } while (0);                                \
    })
static inline umword_t arch_get_sp(void)
{
    return get_sp();
}
umword_t arch_get_isr_no(void);

static inline void arch_set_knl_sp(umword_t sp)
{
    set_sp(sp);
}
static inline umword_t arch_get_knl_sp(void)
{
    return get_sp();
}
static inline void arch_set_user_sp(umword_t sp)
{
}
static inline umword_t arch_get_user_sp(void)
{
    return 0;
}
void arch_disable_irq(int inx);
void arch_enable_irq(int inx);
void arch_set_enable_irq_prio(int inx, int sub_prio, int pre_prio);

/**
 * 开中断
 */
#define sti()                 \
    do {                      \
        asm volatile(         \
            "msr daifclr, #3" \
            :                 \
            :                 \
            : "memory");      \
    } while (0)

/**
 * 关中断
 */
#define cli()                 \
    do {                      \
        asm volatile(         \
            "msr daifset, #3" \
            :                 \
            :                 \
            : "memory");      \
    } while (0)

static inline __attribute__((optimize(0))) void preemption(void)
{
    sti();
    cli();
}

static inline umword_t intr_status(void)
{
    umword_t ret;

    asm volatile("mrs %0, daif" : "=r"(ret));
    return !!(ret & 0xc0);
}

void sys_startup(void);
void sys_reset(void);

// systick.c
umword_t sys_tick_cnt_get(void);

uint32_t arch_get_sys_clk(void);
