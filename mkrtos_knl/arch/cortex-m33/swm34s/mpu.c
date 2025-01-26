/*
 * mpu.c
 *
 *  Created on: ATShining
 *      Author: Administrator
 */

#include "types.h"
#include "assert.h"
#include "mpu.h"
#include <mk_sys.h>
#include <mpu_armv8.h>
#include "thread.h"
#include "task.h"
#if CONFIG_MPU

void mpu_init(void)
{
    mpu_enable();
}
void mpu_disable(void)
{
    ARM_MPU_Disable();
}
void mpu_enable(void)
{
    ARM_MPU_Enable(4);
}
void mpu_calc_regs(region_info_t *region, umword_t addr, umword_t ffs_val, uint8_t attrs, uint8_t regions_bits)
{
    assert((addr & (MPU_ALIGN_SIZE - 1)) == 0);
    assert((ffs_val & (MPU_ALIGN_SIZE - 1)) == 0);
    region->rbar = ARM_MPU_RBAR(addr, ARM_MPU_SH_NON,
                                0, 0, attrs == REGION_RWX ? 0 : 1);
    region->rasr = ARM_MPU_RLAR(addr + ffs_val, region->region_inx);
    ARM_MPU_SetMemAttr(region->region_inx, ARM_MPU_ATTR_NON_CACHEABLE);
}
void mpu_region_set(int inx, umword_t rbar, umword_t rasr)
{
    mpu_disable();
    ARM_MPU_SetRegion(inx, rbar, rasr);
    mpu_enable();
}
void mpu_region_clr(int inx)
{
    ARM_MPU_ClrRegion(inx);
}
void mpu_switch_to_task(struct task *tk)
{
    mpu_disable();
    for (int i = 0; i < CONFIG_REGION_NUM; i++)
    {
        if (tk->mm_space.pt_regions[i].region_inx >= 0)
        {
            ARM_MPU_SetRegion(tk->mm_space.pt_regions[i].region_inx, tk->mm_space.pt_regions[i].rbar,
                              tk->mm_space.pt_regions[i].rasr);
        }
        else
        {
            ARM_MPU_ClrRegion(i);
        }
    }
    mpu_enable();
}
void mpu_switch_to(void)
{
    struct task *tk = thread_get_current_task();

    mpu_switch_to_task(tk);
}
#else
void mpu_switch_to(void)
{
}
void mpu_switch_to_task(struct task *tk)
{
}
#endif
