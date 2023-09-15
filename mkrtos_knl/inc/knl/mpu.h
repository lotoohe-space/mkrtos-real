#pragma once
#include "mm_space.h"
#include "task.h"
void mpu_init(void);
void mpu_disable(void);
void mpu_enable(void);
void mpu_region_set(int inx, umword_t rbar, umword_t rasr);
void mpu_region_clr(int inx);
void mpu_calc_regs(region_info_t *region, umword_t addr, int ffs,
                   uint8_t attrs, uint8_t regions_bits);
void mpu_switch_to(void);
void mpu_switch_to_task(struct task *tk);