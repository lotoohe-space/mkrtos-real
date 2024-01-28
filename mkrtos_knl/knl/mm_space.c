/**
 * @file mm_space.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-09-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "types.h"
#include "util.h"
#include "mm_space.h"
#include "mpu.h"
#include "assert.h"
#if CONFIG_MK_MPU_CFG
void mm_space_init(mm_space_t *mm_space, int is_knl)
{
    for (int i = 0; i < CONFIG_REGION_NUM; i++)
    {
        mm_space->pt_regions[i].region_inx = -1;
    }
}
region_info_t *mm_space_alloc_pt_region(mm_space_t *m_space)
{
    for (int i = 0; i < CONFIG_REGION_NUM; i++)
    {
        /*TODO:原子操作*/
        if (m_space->pt_regions[i].region_inx < 0)
        {
            m_space->pt_regions[i].region_inx = (int16_t)i;
            return &m_space->pt_regions[i];
        }
    }
    return NULL;
}
void mm_space_free_pt_region(mm_space_t *m_space, region_info_t *ri)
{
    ri->region_inx = -1;
}

bool_t mm_space_add(mm_space_t *m_space,
                    umword_t addr,
                    umword_t size,
                    uint8_t attrs)
{
    region_info_t *ri = mm_space_alloc_pt_region(m_space);

    if (!ri)
    {
        return FALSE;
    }
#if CONFIG_MPU_VERSION == 1
    if (!is_power_of_2(size) || (addr & (!(size - 1))) != 0)
    {
        //!< 申请的大小必须是2的整数倍，而且地址也必须是2的整数倍
        mm_space_free_pt_region(m_space, ri);
        return FALSE;
    }
#elif CONFIG_MPU_VERSION == 2
    if ((size & (MPU_ALIGN_SIZE - 1)) == 0 && (addr & (MPU_ALIGN_SIZE - 1)) == 0)
    {
        mm_space_free_pt_region(m_space, ri);
        return FALSE;
    }
#endif
    mpu_calc_regs(ri, addr, size, attrs, 0);
    ri->start_addr = addr;
    ri->size = size;
    return TRUE;
}
void mm_space_del(mm_space_t *m_space, umword_t addr)
{
    for (int i = 0; i < CONFIG_REGION_NUM; i++)
    {
        if (m_space->pt_regions[i].region_inx >= 0 &&
            m_space->pt_regions[i].start_addr == addr)
        {
            m_space->pt_regions[i].region_inx = -1;
            m_space->pt_regions[i].start_addr = 0;
            break;
        }
    }
}
#endif
