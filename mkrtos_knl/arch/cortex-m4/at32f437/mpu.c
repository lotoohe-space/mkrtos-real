/*
 * mpu.c
 *
 *  Created on: ATShining
 *      Author: Administrator
 */
#include "types.h"
#include "mpu.h"
#include <mk_sys.h>
#include <mpu_armv7.h>
#include "thread.h"
#include "task.h"
#if IS_ENABLED(CONFIG_MK_MPU_CFG)
static volatile umword_t *MPUCR = (umword_t *)0xE000ED94;

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
void mpu_calc_regs(region_info_t *region, umword_t addr, umword_t size, uint8_t attrs, uint8_t regions_bits)
{
    region->rbar = ARM_MPU_RBAR(region->region_inx, addr);
    region->rasr = ARM_MPU_RASR(0, attrs, 0UL, 0UL,
                                1UL, 1UL, regions_bits, ffs(size) - 1);
}
void mpu_region_set(int inx, umword_t rbar, umword_t rasr)
{
    mpu_disable();
    ARM_MPU_SetRegionEx(inx, rbar, rasr);
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
        region_info_t *ri = &tk->mm_space.mem_vma.pt_regions[i];
        if (ri->region_inx >= 0)
        {
            ARM_MPU_SetRegionEx(ri->region_inx, ri->rbar, ri->rasr);
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
