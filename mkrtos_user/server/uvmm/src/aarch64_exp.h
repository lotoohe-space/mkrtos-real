#pragma once

#include "u_types.h"
#include "cpu_regs.h"

int aarch64_exp_sync(cpu_regs_t *regs, umword_t esr, umword_t fipa);
