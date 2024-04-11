#pragma once
#include <arch.h>

void cpu_switch_to(sp_info_t *prev,
                   sp_info_t *next);
void sche_arch_sw_context(void);
