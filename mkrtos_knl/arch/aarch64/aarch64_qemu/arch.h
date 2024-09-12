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
#include <asm/mm.h>
#include <mmu.h>
#include <ipi.h>

#define ARCH_WORD_SIZE 64
#define SYSTICK_INTR_NO 30
#define LOG_INTR_NO 33

#define CPU0_MASK (1 << 0)
#define CPU1_MASK (1 << 1)
#define CPU2_MASK (1 << 2)
#define CPU3_MASK (1 << 3)
#define CPU4_MASK (1 << 4)
#define CPU5_MASK (1 << 5)
#define CPU6_MASK (1 << 6)
#define CPU7_MASK (1 << 7)

/// @brief 线程信息
typedef struct
{
    umword_t rg0[8]; //!< r0-r3
    umword_t r12;
    umword_t lr;
    umword_t pc;
} pf_s_t;
typedef struct pf
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
} pf_t;

typedef struct sysregs
{
    umword_t hstr_el2;

    umword_t sp_el0;
    umword_t sctlr_el1;
    umword_t ttbr0_el1;
    umword_t ttbr1_el1;
    umword_t tcr_el1;
    umword_t esr_el1;
    umword_t far_el1;
    umword_t mair_el1;
    umword_t contextidr_el1;
    umword_t tpidrro_el0;

    umword_t vmpidr_el2;

    // 32bits sysregs.
    umword_t spsr_abt;
    umword_t spsr_und;
    umword_t spsr_irq;
    umword_t spsr_fiq;
    umword_t dacr32_el2;
    umword_t ifsr32_el2;

    umword_t teecr32_el1;
    umword_t teehbr32_el1;

    umword_t fpexc32_el2;
    umword_t fpcr;
    umword_t fpsr;

    umword_t q[32];

    umword_t cntp_ctl_el0;
    umword_t cntv_ctl_el0;
    umword_t cntkctl_el1;
    umword_t cntp_cval_el0;
    umword_t cntv_cval_el0;

} sysregs_t;
typedef struct hyp_sysregs
{
    umword_t par_el1;
    umword_t hcr_el2;
    umword_t cntv_cval_el0;
    umword_t cntkctl_el1;
    umword_t cntv_ctl_el0;
    umword_t tpidr_el1;
    umword_t sp_el1;
    umword_t elr_el1;
    umword_t vbar_el1;
    umword_t cpacr_el1;
    umword_t spsr_fiq;
    umword_t spsr_irq;
    umword_t spsr_el1;
    umword_t spsr_abt;
    umword_t spsr_und;
    umword_t csselr_el1;
} hyp_sysregs_t;
typedef struct sp_info
{
    umword_t x19;
    umword_t x20;
    umword_t x21;
    umword_t x22;
    umword_t x23;
    umword_t x24;
    umword_t x25;
    umword_t x26;
    umword_t x27;
    umword_t x28;
    umword_t fp; // x29
    umword_t sp;
    umword_t pc;
   // umword_t u_sp; // user_sp
   // umword_t tpidr_el0;

    hyp_sysregs_t hyp;
#if IS_ENABLED(CONFIG_VCPU)
    sysregs_t sysregs;
#endif
} sp_info_t;

#define flush_all_tlb()  \
    do { \
        /*asm volatile("tlbi alle2");*/\
        asm volatile("tlbi alle1");\
    } while (0)
#define cpu_sleep() asm volatile("wfi" : : : "memory")
#define _barrier() __asm__ __volatile__("" : : : "memory")
#define _dmb(ins) \
    asm volatile("dmb " #ins : : : "memory")
#define _isb() asm volatile("isb" : : : "memory")
#define _dsb(ins) \
    asm volatile("dsb " #ins : : : "memory")

#define __arch_getl(a) (*(volatile unsigned int *)(a))
#define __arch_putl(v, a) (*(volatile unsigned int *)(a) = (v))

#define __iormb() _barrier()
#define __iowmb() _barrier()

#define readl(c) ({ unsigned int  __v = __arch_getl(c); __iormb(); __v; })
#define writel(v, c) ({ unsigned int  __v = v; __iowmb(); __arch_putl(__v,c); })

#define read_reg(addr)                           \
    ({                                           \
        unsigned long _val;                      \
                                                 \
        _barrier();                              \
        _val = (*((volatile umword_t *)(addr))); \
        _val;                                    \
    })
#define write_reg(addr, data)                    \
    do                                           \
    {                                            \
        _barrier();                              \
        (*((volatile umword_t *)(addr))) = data; \
        _barrier();                              \
    } while (0)

#define read_reg32(addr)                         \
    ({                                           \
        unsigned long _val;                      \
                                                 \
        _barrier();                              \
        _val = (*((volatile umword_t *)(addr))); \
        _val;                                    \
    })
#define write_reg32(addr, data)                            \
    do                                                     \
    {                                                      \
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

#define read_nmreg(reg) ({      \
    unsigned long _val;          \
    asm volatile("mov %0, " #reg \
                 : "=r"(_val));  \
    _val;                        \
})

static inline uint64_t arch_get_currentel(void)
{
    unsigned long _val;

    asm volatile("mrs %0, CurrentEL"
                 : "=r"(_val));
    return _val >> 2;
}
static inline int arch_get_current_cpu_id(void)
{
    return read_sysreg(mpidr_el1) & 0XFFUL;
}

#define is_run_knl() \
    ({/*TODO:*/      \
      0})

void arch_to_sche(void);

#define get_sp() (                                 \
    {                                              \
        mword_t sp;                                \
        do                                         \
        {                                          \
            asm volatile("mov %0, sp" : "=r"(sp)); \
        } while (0);                               \
        sp;                                        \
    })
#define set_sp(sp) (                                \
    {                                               \
        do                                          \
        {                                           \
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
    do                        \
    {                         \
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
    do                        \
    {                         \
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

#include <sche_arch.h>
