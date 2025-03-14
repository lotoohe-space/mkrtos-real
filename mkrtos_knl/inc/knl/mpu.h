#pragma once
#include "mm_space.h"
#include "task.h"

#if CONFIG_MPU
#if CONFIG_MPU_VERSION == 2
#define MPU_ALIGN_SIZE 32
#endif

void mpu_init(void);
void mpu_disable(void);
void mpu_enable(void);
void mpu_region_set(int inx, umword_t rbar, umword_t rasr);
void mpu_region_clr(int inx);
void mpu_calc_regs(region_info_t *region, umword_t addr, umword_t ffs_val,
                   uint8_t attrs, uint8_t regions_bits);
void mpu_switch_to(void);
void mpu_switch_to_task(struct task *tk);
static inline umword_t vpage_attrs_to_page_attrs(enum vpage_prot_attrs attrs)
{
    umword_t to_attrs = 0;

    if (attrs & VPAGE_PROT_RO)
    {
        to_attrs = REGION_RO;
    }
    if ((attrs & VPAGE_PROT_RW) == VPAGE_PROT_RW)
    {
        to_attrs = REGION_RWX;
    }
    if ((attrs & VPAGE_PROT_RWX) == VPAGE_PROT_RWX)
    {
        to_attrs = REGION_RWX;
    }
    return to_attrs;
}
#else
#if !IS_ENABLED(CONFIG_MMU)
static inline void mpu_init(void)
{
}
static inline void mpu_disable(void)
{
}
static inline void mpu_enable(void)
{
}
static inline void mpu_calc_regs(region_info_t *region, umword_t addr, umword_t ffs_val,
                                 uint8_t attrs, uint8_t regions_bits)
{
}
static inline void mpu_region_set(int inx, umword_t rbar, umword_t rasr)
{
}

static inline void mpu_region_clr(int inx)
{
}
void mpu_switch_to(void);
void mpu_switch_to_task(struct task *tk);
static inline umword_t vpage_attrs_to_page_attrs(enum vpage_prot_attrs attrs)
{
    return 0xff;
}
#else
static inline void mpu_switch_to_task(struct task *tk)
{
}
#endif
#endif
