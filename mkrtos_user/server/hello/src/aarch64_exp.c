
#include "aarch64_exp.h"
#include "cpu_def.h"
#include "u_types.h"
#include "guest_os.h"
#include "aarch64_emul.h"
#include "cpu_regs.h"
#include <errno.h>
#include <stdio.h>
static int cpu_vcpu_data_abort(guest_vcpu_t *vcpu,
                               cpu_regs_t *regs,
                               uint32_t il, uint32_t iss,
                               paddr_t fipa);
int aarch64_exp_sync(cpu_regs_t *regs, umword_t esr, umword_t fipa)
{
    uint32_t ec, il, iss;
    int rc = -1;
    // obj_handler_t vcpu = 0; /*TODO:vcpu*/
    guest_vcpu_t *vcpu = (guest_vcpu_t *)(thread_get_cur_ipc_msg()->user[3]);

    ec = (esr & ESR_EC_MASK) >> ESR_EC_SHIFT;
    il = (esr & ESR_IL_MASK) >> ESR_IL_SHIFT;
    iss = (esr & ESR_ISS_MASK) >> ESR_ISS_SHIFT;

    if (ec == EC_TRAP_LWREL_DATA_ABORT)
    {
        /* Stage2 data abort */
        rc = cpu_vcpu_data_abort(vcpu, regs, il, iss, fipa);
    }
    return rc;
}
static int cpu_vcpu_data_abort(guest_vcpu_t *vcpu,
                               cpu_regs_t *regs,
                               uint32_t il, uint32_t iss,
                               paddr_t fipa)
{
    uint32_t read_count, inst;
    paddr_t inst_pa;

    switch (iss & ISS_ABORT_FSC_MASK)
    {
    case FSC_TRANS_FAULT_LEVEL1:
    case FSC_TRANS_FAULT_LEVEL2:
    case FSC_TRANS_FAULT_LEVEL3:
        // 翻译错误应该在内核中处理，不会走到这里来
        // return cpu_vcpu_stage2_map(vcpu, regs, fipa);
        return -EACCES;
    case FSC_ACCESS_FAULT_LEVEL1:
    case FSC_ACCESS_FAULT_LEVEL2:
    case FSC_ACCESS_FAULT_LEVEL3:
        if (!(iss & ISS_ABORT_ISV_MASK))
        {
#if 0
            // 指令错误
            /* Determine instruction physical address */
            va2pa_at(VA2PA_STAGE12, VA2PA_EL1, VA2PA_RD, regs->pc);
            inst_pa = mrs(par_el1);
            inst_pa &= PAR_PA_MASK;
            inst_pa |= (regs->pc & 0x00000FFF);

            /* Read the faulting instruction */
            /* FIXME: Should this be cacheable memory access ? */
            read_count = vmm_host_memory_read(inst_pa,
                                              &inst, sizeof(inst), TRUE);
            if (read_count != sizeof(inst))
            {
                return -EFAULT;
            }
            if (regs->pstate & PSR_THUMB_ENABLED)
            {
                return emulate_thumb_inst(vcpu, regs, inst);
            }
            else
            {
                return emulate_arm_inst(vcpu, regs, inst);
            }
#else
            return -EFAULT;
#endif
        }
        // 数据访问错误
        if (iss & ISS_ABORT_WNR_MASK)
        {
            return cpu_vcpu_emulate_store(vcpu, regs,
                                          il, iss, fipa);
        }
        else
        {
            return cpu_vcpu_emulate_load(vcpu, regs,
                                         il, iss, fipa);
        }
    default:
        printf("%s: Unhandled FSC=0x%x\n",
               __func__, iss & ISS_ABORT_FSC_MASK);
        break;
    };

    return -EFAULT;
}
