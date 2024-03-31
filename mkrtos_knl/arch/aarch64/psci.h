#pragma once

#include <types.h>

void psci_init(void);
umword_t psci_cpu_on(umword_t cpuid, umword_t entry_point);
void psci_system_reset(void);
void psic_system_off(void);
void arm_smc(umword_t arg0,
             umword_t arg1,
             umword_t arg2,
             umword_t arg3,
             umword_t arg4,
             umword_t arg5,
             umword_t arg6,
             umword_t arg7);
