#include <printk.h>
#include <arch.h>
static const char *const bad_mode_handler[] = {
    "Sync Abort",
    "IRQ",
    "FIQ",
    "SError"};

void bad_mode(struct pt_regs *regs, int reason, unsigned int esr)
{
    printk("Bad mode for %s handler detected, esr=0x%x ec=0x%x far_el2=0x%lx\n",
           bad_mode_handler[reason], esr, esr >> 26, read_sysreg(far_el2));

    mword_t x29;

    asm volatile("mov %0, x29" : "=r"(x29));
    printk("x29 %lx.\n", x29);
    for (size_t i = 0; i < 5; i++)
    {
        x29 = *(long *)(x29);
        printk("kernel sync instruction addr is %lx\n", *(long *)(x29 + 8));
    }
    while (1)
        ;
}
