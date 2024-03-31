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
#include "arch.h"
#include <thread_arch.h>
#include <aarch64_ptregs.h>
#define LOG_INTR_NO 37 // USART1_IRQn

#define _barrier() __asm__ __volatile__("" : : : "memory")
#define _dmb(ins) \
    asm volatile("dmb " #ins : : : "memory")
#define _isb() asm volatile("isb" : : : "memory")
#define _dsb(ins) \
    asm volatile("dsb " #ins : : : "memory")

#define read_reg(addr) (*((volatile umword_t *)(addr)))
#define write_reg(addr, data)                    \
    do                                           \
    {                                            \
        _barrier();                              \
        (*((volatile umword_t *)(addr))) = data; \
        _barrier();                              \
    } while (0)

#define read_reg32(addr) (*((volatile uint32_t *)(addr)))
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
    ({               \
        /*TODO:*/    \
    })

void to_sche(void);

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
static inline umword_t arch_get_isr_no(void)
{
    /*TODO:获取中断号*/
    return 0;
}
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
    return !(ret & 0xc0);
}

void sys_startup(void);
void sys_reset(void);

// systick.c
umword_t sys_tick_cnt_get(void);

uint32_t arch_get_sys_clk(void);
