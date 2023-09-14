/*
 * @Author: zhangzheng 1358745329@qq.com
 * @Date: 2023-08-18 15:03:16
 * @LastEditors: zhangzheng 1358745329@qq.com
 * @LastEditTime: 2023-08-18 16:24:47
 * @FilePath: /mkrtos-real/mkrtos_knl/arch/armv7m/arch.c
 * @Description: 与arch相关的借口
 */

#include "arch.h"
#include "types.h"
#include "util.h"
#include "init.h"
#include "config.h"
#include "thread.h"
#include "stm32f2xx_conf.h"
#include "mpu.h"
#include "core_cm3.h"
__ALIGN__(THREAD_BLOCK_SIZE)
static uint8_t thread_knl_stack[THREAD_BLOCK_SIZE] = {0};
void *_estack = thread_knl_stack + THREAD_BLOCK_SIZE;

#define REG0_ADDR 0xE000ED22
#define REG1_ADDR 0xE000ED04

void to_sche(void)
{
    // 开启pensv中断
    write_reg(REG1_ADDR, 0x10000000);
}
void sys_startup(void)
{
    // 设置 systick 与 pendsv优先级为最低
    write_reg(REG0_ADDR, MASK_LSB(read_reg(REG0_ADDR), 16) | 0xffffUL);
    // 清空psp寄存器
    write_sysreg(0, psp);
    // 开启pensv中断
    write_reg(REG1_ADDR, 0x10000000);
    // 初始化systick时钟
    SysTick_Config(SystemCoreClock / SYS_SCHE_HZ);
}

void arch_disable_irq(int inx)
{
    NVIC_DisableIRQ(inx);
}
void arch_enable_irq(int inx)
{
    NVIC_EnableIRQ(inx);
}

void arch_init(void)
{
    SystemInit();
    mpu_init();
    SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk;
    SCB->SHCSR |= SCB_SHCSR_BUSFAULTENA_Msk;
    ((uint8_t *)(0xE000E008))[0] |= 0x6;
}
INIT_LOW_HARD(arch_init);
