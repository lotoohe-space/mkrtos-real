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
#include <psci.h>
#include <arm_gicv2.h>
#include <timer/timer.h>
#include <hyp.h>
#include <sche_arch.h>
__ALIGN__(THREAD_BLOCK_SIZE)
static uint8_t thread_knl_stack[THREAD_BLOCK_SIZE] = {0};
void *_estack = thread_knl_stack + THREAD_BLOCK_SIZE;

/**
 * 进行调度
 */
void to_sche(void)
{
    sche_arch_sw_context();
}
/**
 * 进行一些系统的初始化
 */
void sys_startup(void)
{
    /*TODO:*/
    init_arm_hyp();
    timer_init(arch_get_current_cpu_id());
}
void sys_reset(void)
{
    /*TODO:*/
}
void arch_disable_irq(int inx)
{
    /*TODO:*/
}
void arch_enable_irq(int inx)
{
    /*TODO:*/
}
uint32_t arch_get_sys_clk(void)
{
    /*TODO:*/
    return 0;
}
void arch_set_enable_irq_prio(int inx, int sub_prio, int pre_prio)
{
    /*TODO:*/
}
extern char _data_boot[], _edata_boot[];
extern char _text_boot[], _etext_boot[];
extern char _text[], _etext[];
extern char _rodata[], _erodata[];
extern char _data[], _edata[];
extern char _bss[], _ebss[];
extern char _buddy_data_start[];
extern char _buddy_data_end[];

static void print_mem(void)
{
    printk("MKRTOS image layout:\n");
    printk("  .text.boot: 0x%08lx - 0x%08lx (%6ld B)\n",
           (unsigned long)_text_boot, (unsigned long)_etext_boot,
           (unsigned long)(_etext_boot - _text_boot));
    printk("  .text.data: 0x%08lx - 0x%08lx (%6ld B)\n",
           (unsigned long)_data_boot, (unsigned long)_edata_boot,
           (unsigned long)(_edata_boot - _data_boot));
    printk("       .text: 0x%08lx - 0x%08lx (%6ld B)\n",
           (unsigned long)_text, (unsigned long)_etext,
           (unsigned long)(_etext - _text));
    printk("     .rodata: 0x%08lx - 0x%08lx (%6ld B)\n",
           (unsigned long)_rodata, (unsigned long)_erodata,
           (unsigned long)(_erodata - _rodata));
    printk("       .data: 0x%08lx - 0x%08lx (%6ld B)\n",
           (unsigned long)_data, (unsigned long)_edata,
           (unsigned long)(_edata - _data));
    printk("        .bss: 0x%08lx - 0x%08lx (%6ld B)\n",
           (unsigned long)_bss, (unsigned long)_ebss,
           (unsigned long)(_ebss - _bss));
    printk("      .buddy: 0x%08lx - 0x%08lx (%6ld B)\n",
           (unsigned long)_buddy_data_start, (unsigned long)_buddy_data_end,
           (unsigned long)(_buddy_data_end - _buddy_data_start));
}

void arch_init(void)
{
    printk("MKRTOS running on EL:%d\n", arch_get_currentel() >> 2);
    print_mem();
    psci_init();
    gic_init(arm_gicv2_get_global(),
             0x08000000, 0x08010000); /*TODO:*/
}
INIT_LOW_HARD(arch_init);
