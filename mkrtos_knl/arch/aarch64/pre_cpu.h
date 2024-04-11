
#pragma once

#include <util.h>

#define PER_CPU(type, name) \
    __attribute__((used))   \
    SECTION(".data.per_cpu") type name

void pre_cpu_init(void);
void *pre_get_current_cpu_var(void *mem_addr);
void *pre_get_var_cpu(int cpu_inx, void *mem_addr);
