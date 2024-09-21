
#pragma once

#include <util.h>
#if IS_ENABLED(CONFIG_SMP)
#define PER_CPU(type, name) \
    __attribute__((used))   \
    SECTION(".data.per_cpu") type name
#else
#define PER_CPU(type, name) \
    __attribute__((used))   \
    SECTION(".data") type name
#endif
void pre_cpu_init(void);
bool_t pre_cpu_is_init(void);
void *pre_get_current_cpu_var(void *mem_addr);
void *pre_cpu_get_var_cpu(uint32_t cpu_inx, void *mem_addr);
void *pre_cpu_get_current_cpu_var(void *mem_addr);
