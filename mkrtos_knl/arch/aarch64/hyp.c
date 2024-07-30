#include <arch.h>
#include "hyp.h"
void init_arm_hyp(void)
{
    // asm volatile ("msr VBAR_EL2, %x0" : : "r"(&exception_vector));
    asm volatile("msr VTCR_EL2, %x0" : : "r"((1UL << 31) // RES1
                                             | (Tcr_attribs << 8) | Vtcr_bits));

    asm volatile("msr MDCR_EL2, %x0" : : "r"((mword_t)Mdcr_bits));

    asm volatile("msr SCTLR_EL1, %x0" : : "r"((mword_t)Sctlr_el1_generic));
    asm volatile("msr HCR_EL2, %x0" : : "r"(Hcr_non_vm_bits));
    asm volatile("msr HSTR_EL2, %x0" : : "r"(Hstr_non_vm));

    _dsb(sy);
    _isb();

    // HCPTR
    asm volatile("msr CPTR_EL2, %x0" : : "r"(0x33ffUL       // TCP: 0-9, 12-13
                                             | (1 << 20))); // TTA
}
