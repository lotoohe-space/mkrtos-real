#include <printk.h>
#include <arch.h>
#include <syscall.h>
static const char *const bad_mode_handler[] = {
    "Sync Abort",
    "IRQ",
    "FIQ",
    "SError"};
static void dump_stack(umword_t pc, umword_t x29)
{
    uint64_t result[5];
    printk("pc:0x%x x29:0x%x\n", pc, x29);
    for (size_t i = 0; i < sizeof(result) / sizeof(umword_t); i++)
    {
        result[i] = (*(umword_t *)(x29 + 8)) - 4;
        x29 = *(umword_t *)(x29);
    }
    printk("knl pf stack: 0x%x,0x%x,0x%x,0x%x,0x%x\n", result[0], result[1], result[2], result[3], result[4]);
}
void bad_mode(entry_frame_t *regs)
{
    printk("**********panic*************\n");
    // printk("Bad mode for %s handler detected, esr=0x%x ec=0x%x far_el2=0x%lx\n",
    //        bad_mode_handler[reason], esr, esr >> 26, read_sysreg(far_el2));

    dump_stack(regs->pc, regs->regs[29]);
    while (1)
        ;
}
