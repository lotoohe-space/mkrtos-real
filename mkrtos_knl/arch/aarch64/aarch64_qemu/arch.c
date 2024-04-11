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
#include <timer.h>
#include <hyp.h>
#include <sche_arch.h>
#include <spin_table.h>
#include <thread_knl.h>
__ALIGN__(THREAD_BLOCK_SIZE)
static uint8_t thread_knl_stack[CONFIG_CPU][THREAD_BLOCK_SIZE] = {0};
void *_estack = &thread_knl_stack[0] + THREAD_BLOCK_SIZE;
static void other_cpu_boot(void);
extern void _start(void);
/**
 * 进行调度
 */
void to_sche(void)
{
    gic2_set_pending(arm_gicv2_get_global(), SYSTICK_INTR_NO);
    // sche_arch_sw_context();
}
/**
 * 进行一些系统的初始化
 */
void sys_startup(void)
{
    timer_init(arch_get_current_cpu_id());
    for (int i = 1; i < CONFIG_CPU; i++)
    {
        cpu_start_to(i, thread_knl_stack[i], other_cpu_boot);
        psci_cpu_on(i, (umword_t)_start);
    }
}
void sys_reset(void)
{
    psci_system_reset();
}
umword_t arch_get_isr_no(void)
{
    return gicv2_get_irqnr(arm_gicv2_get_global());
}
void arch_disable_irq(int inx)
{
    gic2_set_mask(arm_gicv2_get_global(), inx);
}
void arch_enable_irq(int inx)
{
    gic2_set_unmask(arm_gicv2_get_global(), inx);
}
uint32_t arch_get_sys_clk(void)
{
    /*TODO:*/
    return 0;
}
void arch_set_enable_irq_prio(int inx, int sub_prio, int pre_prio)
{
    gic2_set_prio(arm_gicv2_get_global(), inx, pre_prio);
}
extern char _data_boot[], _edata_boot[];
extern char _text_boot[], _etext_boot[];
extern char _text[], _etext[];
extern char _rodata[], _erodata[];
extern char _data[], _edata[];
extern char _bss[], _ebss[];
extern char _buddy_data_start[];
// extern char _buddy_data_end[];

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
    printk("      .buddy: 0x%08lx - \n",
           (unsigned long)_buddy_data_start);
}

void arch_init(void)
{
    printk("MKRTOS running on EL:%d\n", arch_get_currentel());
    print_mem();
    init_arm_hyp();
    psci_init();
    gic_init(arm_gicv2_get_global(),
             0x08000000, 0x8010000); /*TODO:*/
}
INIT_LOW_HARD(arch_init);
static void arch_cpu_knl_init(void)
{
    init_arm_hyp();
    scheduler_init(); //!< 初始化其他cpu的调度队列
    gic_init(arm_gicv2_get_global(), 0x08000000, 0x8010000);
    knl_init_1();
    timer_init(arch_get_current_cpu_id());
}
static void other_cpu_boot(void)
{
    cli();
    per_cpu_boot_mapping(FALSE);

    mword_t elx = arch_get_currentel();

    printk("cpuid %d run el%d.\n", arch_get_current_cpu_id(), elx);
    arch_cpu_knl_init();
    sti();
    while (1)
        ;
}
