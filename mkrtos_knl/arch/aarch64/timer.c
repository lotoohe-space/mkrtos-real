#include <asm/io.h>
#include <asm/arm_local_reg.h>
#include <asm/timer.h>
#include <arm_gicv2.h>
#include <irq.h>
#include <assert.h>
#include "timer.h"
#define GENERIC_TIMER_IRQ SYSTICK_INTR_NO

static unsigned int arch_timer_rate;

static int generic_timer_init(void)
{
	asm volatile(
		"mov x0, #1\n"
		"msr cntp_ctl_el0, x0"
		:
		:
		: "memory");

	return 0;
}

static int generic_timer_reset(unsigned int val)
{
	asm volatile(
		"msr cntp_tval_el0, %x[timer_val]"
		:
		: [timer_val] "r"(val)
		: "memory");

	return 0;
}

static unsigned int generic_timer_get_freq(void)
{
	unsigned int freq;

	asm volatile(
		"mrs %0, cntfrq_el0"
		: "=r"(freq)
		:
		: "memory");

	return freq;
}

static void enable_timer_interrupt(int inx)
{
	switch (inx)
	{
	case 0:
		writel(CNT_PNS_IRQ, TIMER_CNTRL0);
		break;
	case 1:
		writel(CNT_PNS_IRQ, TIMER_CNTRL1);
		break;
	case 2:
		writel(CNT_PNS_IRQ, TIMER_CNTRL2);
		break;
	case 3:
		writel(CNT_PNS_IRQ, TIMER_CNTRL3);
		break;
	}
}
void timer_init(int cpu)
{
	irq_alloc(GENERIC_TIMER_IRQ, NULL, systick_handler);

	arch_timer_rate = generic_timer_get_freq();
	arch_timer_rate /= CONFIG_SYS_SCHE_HZ;

	generic_timer_init();
	generic_timer_reset(arch_timer_rate);

	gic2_set_unmask(arm_gicv2_get_global(), GENERIC_TIMER_IRQ);
	gic2_set_target_cpu(arm_gicv2_get_global(), GENERIC_TIMER_IRQ, 1 << cpu);
	printk("cpu:%d timer dis:0x%x\n", cpu,
		   gic2_get_target_cpu(arm_gicv2_get_global(), GENERIC_TIMER_IRQ));
}
void handle_timer_irq(void)
{
	generic_timer_reset(arch_timer_rate);
}
