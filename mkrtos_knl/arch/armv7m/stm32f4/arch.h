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
#define LOG_INTR_NO 37//USART1_IRQn

#define read_reg(addr) (*((volatile umword_t *)(addr)))
#define write_reg(addr, data)                    \
    do                                           \
    {                                            \
        (*((volatile umword_t *)(addr))) = data; \
    } while (0)

//! 读取系统寄存器
#define read_sysreg(reg) ({      \
    umword_t _val;               \
    asm volatile("mrs %0, " #reg \
                 : "=r"(_val));  \
    _val;                        \
})

//! 写入系统寄存器
#define write_sysreg(val, reg) ({                         \
    umword_t _val = (umword_t)val;                        \
    __asm__ __volatile__("msr " #reg ", %0" ::"r"(_val)); \
})

#define is_run_knl()           \
    ({                         \
        umword_t ret;          \
        __asm__ __volatile__(  \
            "mov     %0, lr\n" \
            : "=r"(ret)        \
            :                  \
            :);                \
        ((ret & 0x4) ? 0 : 1);   \
    })

void to_sche(void);

static inline umword_t arch_get_sp(void)
{
    umword_t ret;
    __asm__ __volatile__(
        "mov     %0, sp\n"
        : "=r"(ret)
        :
        :);
    return ret;
}
static inline umword_t arch_get_isr_no(void)
{
    umword_t num;
    __asm__ __volatile__(
        "mrs %0,IPSR"
        : "=r"(num)
        :
        :);
    return num;
}
static inline void arch_set_knl_sp(umword_t sp)
{
    write_sysreg(sp, msp);
}
static inline umword_t arch_get_knl_sp(void)
{
    return read_sysreg(msp);
}
static inline void arch_set_user_sp(umword_t sp)
{
    write_sysreg(sp, psp);
}
static inline umword_t arch_get_user_sp(void)
{
    return read_sysreg(psp);
}
void arch_disable_irq(int inx);
void arch_enable_irq(int inx);
void arch_set_enable_irq_prio(int inx, int sub_prio, int pre_prio);

#define sti()                                 \
    do                                        \
    {                                         \
        __asm__ __volatile__("CPSID   I\n" :: \
                                 :);          \
    } while (0)
#define cli()                     \
    do                            \
    {                             \
        write_sysreg(0, PRIMASK); \
    } while (0)

static inline void preemption(void)
{
    cli();
    sti();
}

static inline umword_t intr_status(void)
{
    return read_sysreg(PRIMASK);
}

void sys_startup(void);
void sys_reset(void);

// systick.c
umword_t sys_tick_cnt_get(void);
