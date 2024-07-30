#pragma once

#include "u_hd_man.h"
#include "guest_os.h"
#include "cpu_regs.h"
#include "cpu_def.h"
int cpu_vcpu_emulate_store(guest_vcpu_t *vcpu,
                           cpu_regs_t *regs,
                           uint32_t il, uint32_t iss,
                           paddr_t ipa);
int cpu_vcpu_emulate_load(guest_vcpu_t *vcpu,
                          cpu_regs_t *regs,
                          uint32_t il, uint32_t iss,
                          paddr_t ipa);
