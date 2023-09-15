/*
 * @Author: zhangzheng 1358745329@qq.com
 * @Date: 2023-08-18 15:03:16
 * @LastEditors: zhangzheng 1358745329@qq.com
 * @LastEditTime: 2023-08-18 16:31:06
 * @FilePath: /mkrtos-real/mkrtos_knl/arch/inc/arch.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include "types.h"
#include "arch.h"

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
static inline umword_t intr_status(void)
{
    return read_sysreg(PRIMASK);
}
void sys_startup(void);
// systick.c
umword_t sys_tick_cnt_get(void);
