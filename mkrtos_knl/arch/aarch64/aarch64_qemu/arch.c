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
#include "config.h"
#include "init.h"
#include "mk_sys.h"
#include "thread.h"
#include "types.h"
#include "util.h"
#include <arm_gicv2.h>
#include <hyp.h>
#include <psci.h>
#include <sche_arch.h>
#include <spin_table.h>
#include <thread_knl.h>
#include <timer.h>
#include <ipi.h>
#include <mm_space.h>
__ALIGN__(CONFIG_THREAD_BLOCK_SIZE)
uint8_t thread_knl_stack[CONFIG_CPU][CONFIG_THREAD_BLOCK_SIZE] = {0};
// void *_estack = &thread_knl_stack[0] + CONFIG_THREAD_BLOCK_SIZE;
static umword_t cpu_boot_cn = 0;
static void other_cpu_boot(void);
extern void _start(void);
/**
 * 进行调度
 */
void arch_to_sche(void)
{
    umword_t status;

    status = spinlock_lock(&arm_gicv2_get_global()->lock);
    gic2_set_pending(arm_gicv2_get_global(), SYSTICK_INTR_NO);
    spinlock_set(&arm_gicv2_get_global()->lock, status);
}
/**
 * 进行一些系统的初始化
 */
void sys_startup(void)
{
    cpu_boot_cn = 1;
    cpu_ipi_init();
    for (int i = 1; i < CONFIG_CPU; i++)
    {
        printk("sp:%lx.\n", &thread_knl_stack[i][0]);
        cpu_start_to(i, &thread_knl_stack[i][0] + CONFIG_THREAD_BLOCK_SIZE - MWORD_BYTES, other_cpu_boot);
#if IS_ENABLED(CONFIG_PSCI)
        psci_cpu_on(i, (umword_t)_start);
#endif
    }
    while (cpu_boot_cn < CONFIG_CPU)
        ;
    timer_init(arch_get_current_cpu_id());
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
    return sys_tick_cnt_get();
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
             GIC2_GICD_REG_BASE, GIC2_GICC_REG_BASE);
}
INIT_LOW_HARD(arch_init);
static void arch_cpu_knl_init(void)
{
    cli();
    init_arm_hyp();
    knl_init_1();
    gic_init(arm_gicv2_get_global(), GIC2_GICD_REG_BASE, GIC2_GICC_REG_BASE);
    cpu_ipi_init();
    timer_init(arch_get_current_cpu_id());
    cpu_boot_cn++;
    thread_sched(TRUE);
    sti();
}
static void other_cpu_boot(void)
{
    per_cpu_boot_mapping(FALSE);
    mword_t elx = arch_get_currentel();

    printk("cpuid %d run el%d.\n", arch_get_current_cpu_id(), elx);
    arch_cpu_knl_init();
    while (1)
    {
    }
}
void dumpstack(void)
{
    uint64_t result[5];
    umword_t x29;
    umword_t status = cpulock_lock();

    x29 = read_nmreg(x29);
    for (size_t i = 0; i < sizeof(result) / sizeof(umword_t); i++) {
        result[i] = (*(umword_t *)(x29 + 8)) - 4;
        x29 = *(umword_t *)(x29);
    }
    printk("[knl dump stack]: 0x%lx,0x%lx,0x%lx,0x%lx,0x%lx\n", result[0], result[1], result[2], result[3], result[4]);
    cpulock_set(status);
}
