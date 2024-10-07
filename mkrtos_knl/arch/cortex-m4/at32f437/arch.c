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
#include "boot_info.h"
__ALIGN__(CONFIG_THREAD_BLOCK_SIZE)
static uint8_t thread_knl_stack[CONFIG_THREAD_BLOCK_SIZE] = {0};
void *_estack = thread_knl_stack + CONFIG_THREAD_BLOCK_SIZE;
static boot_info_t boot_info; //!< 启动信息
#define REG0_ADDR 0xE000ED22
#define REG1_ADDR 0xE000ED04

void arch_to_sche(void)
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
uint32_t arch_get_sys_clk(void)
{
    extern uint32_t SystemCoreClock;

    return SystemCoreClock;
}
void arch_set_enable_irq_prio(int inx, int sub_prio, int pre_prio)
{
    nvic_irq_enable(inx, sub_prio, pre_prio);
}
void arch_set_boot_info(void *bi)
{
    boot_info = *((boot_info_t *)bi);

    printk("==============================sys flash layer info============================\n");
    for (int i = 0; i < boot_info.flash_layer.flash_layer_num; i++)
    {
        printk("[%d]\t[0x%x 0x%x]\t%s %dB\n", i,
               boot_info.flash_layer.flash_layer_list[i].st_addr,
               boot_info.flash_layer.flash_layer_list[i].st_addr + boot_info.flash_layer.flash_layer_list[i].size - 1,
               boot_info.flash_layer.flash_layer_list[i].name,
               boot_info.flash_layer.flash_layer_list[i].size);
    }
    printk("=======================================================================\n");
    printk("==============================sys flash info============================\n");
    for (int i = 0; i < boot_info.flash.flash_num; i++)
    {
        printk("[%d]\t[0x%x 0x%x] %dB\n", i,
               boot_info.flash.flash_list[i].addr,
               boot_info.flash.flash_list[i].addr + boot_info.flash.flash_list[i].size - 1,
               boot_info.flash.flash_list[i].size);
    }
    printk("=======================================================================\n");
    printk("==============================sys mem info============================\n");
    for (int i = 0; i < boot_info.mem.mem_num; i++)
    {
        printk("[%d]\t[0x%x 0x%x] %dB\n", i,
               boot_info.mem.mem_list[i].addr,
               boot_info.mem.mem_list[i].addr + boot_info.mem.mem_list[i].size - 1,
               boot_info.mem.mem_list[i].size);
    }
    printk("=======================================================================\n");
}
boot_info_t *arch_get_boot_info(void)
{
    return &boot_info;
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
