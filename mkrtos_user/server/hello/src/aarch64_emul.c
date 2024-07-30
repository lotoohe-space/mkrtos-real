
#include "aarch64_emul.h"
#include "u_hd_man.h"
#include "aarch64_emul.h"
#include "u_types.h"
#include "cpu_def.h"
#include "cpu_regs.h"
#include "guest_os.h"
#include <errno.h>
#include "uvmm_dev_mul.h"


/**
 * Update ITSTATE when emulating instructions inside an IT-block
 *
 * When IO abort occurs from Thumb IF-THEN blocks, the ITSTATE field
 * of the CPSR is not updated, so we do this manually.
 */
static void cpu_vcpu_update_itstate(guest_vcpu_t *vcpu,
                                    cpu_regs_t *regs)
{
    uint32_t itbits, cond;

    if (!(regs->pstate & PSR_IT_MASK))
    {
        return;
    }

    cond = (regs->pstate & 0xe000) >> 13;
    itbits = (regs->pstate & 0x1c00) >> (10 - 2);
    itbits |= (regs->pstate & (0x3 << 25)) >> 25;

    /* Perform ITAdvance (see page A-52 in ARM DDI 0406C) */
    if ((itbits & 0x7) == 0)
        itbits = cond = 0;
    else
        itbits = (itbits << 1) & 0x1f;

    regs->pstate &= ~PSR_IT_MASK;
    regs->pstate |= cond << 13;
    regs->pstate |= (itbits & 0x1c) << (10 - 2);
    regs->pstate |= (itbits & 0x3) << 25;
}
static inline uint32_t arm_sign_extend(uint32_t imm, uint32_t len, uint32_t bits)
{
    if (imm & (1 << (len - 1)))
    {
        imm = imm | (~((1 << len) - 1));
    }
    return imm & ((1 << bits) - 1);
}

void cpu_vcpu_reg64_write(guest_vcpu_t *vcpu,
                          cpu_regs_t *regs,
                          uint32_t reg, uint64_t val)
{
    /* Truncate bits[63:32] for AArch32 mode */
    if (regs->pstate & PSR_MODE32)
    {
        val = val & 0xFFFFFFFFULL;
    }

    if (reg < CPU_GPR_COUNT)
    {
        regs->regs[reg] = val;
    }
    else
    {
        /* No such GPR */
    }
}

uint64_t cpu_vcpu_reg64_read(guest_vcpu_t *vcpu,
                             cpu_regs_t *regs,
                             uint32_t reg)
{
    uint64_t ret;

    if (reg < CPU_GPR_COUNT)
    {
        ret = regs->regs[reg];
    }
    else
    {
        /* No such reg */
        ret = 0;
    }

    /* Truncate bits[63:32] for AArch32 mode */
    if (regs->pstate & PSR_MODE32)
    {
        ret = ret & 0xFFFFFFFFULL;
    }

    return ret;
}
int cpu_vcpu_emulate_load(guest_vcpu_t *vcpu,
                          cpu_regs_t *regs,
                          uint32_t il, uint32_t iss,
                          paddr_t ipa)
{
    int rc;
    uint8_t data8;
    uint16_t data16;
    uint32_t data32, sas, sse, srt;
    enum vmm_devemu_endianness data_endian = VMM_DEVEMU_LITTLE_ENDIAN;

#if 0
	if (regs->pstate & PSR_MODE32) { /* Aarch32 VCPU */
		if (regs->pstate & CPSR_BE_ENABLED) {
			data_endian = VMM_DEVEMU_BIG_ENDIAN;
		} else {
			data_endian = VMM_DEVEMU_LITTLE_ENDIAN;
		}
	} else { /* Aarch64 VCPU */
		if (arm_priv(vcpu)->sysregs.sctlr_el1 & SCTLR_EE_MASK) {
			data_endian = VMM_DEVEMU_BIG_ENDIAN;
		} else {
			data_endian = VMM_DEVEMU_LITTLE_ENDIAN;
		}
	}
#endif
    sas = (iss & ISS_ABORT_SAS_MASK) >> ISS_ABORT_SAS_SHIFT;
    sse = (iss & ISS_ABORT_SSE_MASK) >> ISS_ABORT_SSE_SHIFT;
    srt = (iss & ISS_ABORT_SRT_MASK) >> ISS_ABORT_SRT_SHIFT;

    sse = (sas == 2) ? 0 : sse;

    switch (sas)
    {
    case 0:
        rc = vmm_devemu_emulate_read(vcpu, ipa,
                                     &data8, sizeof(data8),
                                     data_endian);
        if (!rc)
        {
            if (sse)
            {
                cpu_vcpu_reg64_write(vcpu, regs, srt,
                                     arm_sign_extend(data8, 8, 32));
            }
            else
            {
                cpu_vcpu_reg64_write(vcpu, regs, srt, data8);
            }
        }
        break;
    case 1:
        rc = vmm_devemu_emulate_read(vcpu, ipa,
                                     &data16, sizeof(data16),
                                     data_endian);
        if (!rc)
        {
            if (sse)
            {
                cpu_vcpu_reg64_write(vcpu, regs, srt,
                                     arm_sign_extend(data16, 16, 32));
            }
            else
            {
                cpu_vcpu_reg64_write(vcpu, regs, srt, data16);
            }
        }
        break;
    case 2:
        rc = vmm_devemu_emulate_read(vcpu, ipa,
                                     &data32, sizeof(data32),
                                     data_endian);
        if (!rc)
        {
            cpu_vcpu_reg64_write(vcpu, regs, srt, data32);
        }
        break;
    default:
        rc = -EFAULT;
        break;
    };

    if (!rc)
    {
        /* Next instruction */
        regs->pc += (il) ? 4 : 2;
        /* Update ITSTATE for Thumb mode */
        if (regs->pstate & PSR_THUMB_ENABLED)
        {
            cpu_vcpu_update_itstate(vcpu, regs);
        }
    }

    return rc;
}

int cpu_vcpu_emulate_store(guest_vcpu_t *vcpu,
                           cpu_regs_t *regs,
                           uint32_t il, uint32_t iss,
                           paddr_t ipa)
{
    int rc;
    uint8_t data8;
    uint16_t data16;
    uint32_t data32, sas, srt;
    enum vmm_devemu_endianness data_endian = VMM_DEVEMU_LITTLE_ENDIAN;
#if 0

    if (regs->pstate & PSR_MODE32)
    { /* Aarch32 VCPU */
        if (regs->pstate & CPSR_BE_ENABLED)
        {
            data_endian = VMM_DEVEMU_BIG_ENDIAN;
        }
        else
        {
            data_endian = VMM_DEVEMU_LITTLE_ENDIAN;
        }
    }
    else
    { /* Aarch64 VCPU */
        if (arm_priv(vcpu)->sysregs.sctlr_el1 & SCTLR_EE_MASK)
        {
            data_endian = VMM_DEVEMU_BIG_ENDIAN;
        }
        else
        {
            data_endian = VMM_DEVEMU_LITTLE_ENDIAN;
        }
    }
#endif
    sas = (iss & ISS_ABORT_SAS_MASK) >> ISS_ABORT_SAS_SHIFT;
    srt = (iss & ISS_ABORT_SRT_MASK) >> ISS_ABORT_SRT_SHIFT;

    switch (sas)
    {
    case 0:
        data8 = cpu_vcpu_reg64_read(vcpu, regs, srt);
        rc = vmm_devemu_emulate_write(vcpu, ipa,
                                      &data8, sizeof(data8),
                                      data_endian);
        break;
    case 1:
        data16 = cpu_vcpu_reg64_read(vcpu, regs, srt);
        rc = vmm_devemu_emulate_write(vcpu, ipa,
                                      &data16, sizeof(data16),
                                      data_endian);
        break;
    case 2:
        data32 = cpu_vcpu_reg64_read(vcpu, regs, srt);
        rc = vmm_devemu_emulate_write(vcpu, ipa,
                                      &data32, sizeof(data32),
                                      data_endian);
        break;
    default:
        rc = -EFAULT;
        break;
    };

    if (!rc)
    {
        /* Next instruction */
        regs->pc += (il) ? 4 : 2;
        /* Update ITSTATE for Thumb mode */
        if (regs->pstate & PSR_THUMB_ENABLED)
        {
            cpu_vcpu_update_itstate(vcpu, regs);
        }
    }

    return rc;
}
