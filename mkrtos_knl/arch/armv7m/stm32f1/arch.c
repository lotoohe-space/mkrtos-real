/**
 * @file arch.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-09-25
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "arch.h"
#include "types.h"
#include "util.h"
#include "init.h"
#include "config.h"
#include "thread.h"
#include "mk_sys.h"
#include "mpu.h"

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
    SysTick_Config(SystemCoreClock / CONFIG_SYS_SCHE_HZ);
}
void sys_reset(void)
{
    __set_FAULTMASK(1); // 关闭所有中端
    NVIC_SystemReset(); // 复位
}
void arch_disable_irq(int inx)
{
    NVIC_DisableIRQ(inx);
}
void arch_enable_irq(int inx)
{
    NVIC_EnableIRQ(inx);
}
void arch_set_enable_irq_prio(int inx, int sub_prio, int pre_prio)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = inx;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = pre_prio;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = sub_prio;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
void arch_init(void)
{
    SystemInit();
    mpu_init();
    SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk;
    SCB->SHCSR |= SCB_SHCSR_BUSFAULTENA_Msk;
    ((uint8_t *)(0xE000E008))[0] |= 0x6;
    // RCC_ClocksTypeDef RCC_ClocksStatus;
    // RCC_GetClocksFreq(&RCC_ClocksStatus);
}
INIT_LOW_HARD(arch_init);
