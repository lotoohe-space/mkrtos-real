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

void mm_space_init(mm_space_t *mm_space, int is_knl)
{
    region_info_t *regi_info;

    for (int i = 0; i < CONFIG_REGION_NUM; i++)
    {
        mm_space->pt_regions[i].region_inx = -1;
    }
    if (!is_knl)
    {
        regi_info = mm_space_alloc_pt_region(mm_space);
        assert(regi_info);
        // mm_pages_init(&mm_space->mm_pages, regi_info);
    }
}
region_info_t *mm_space_alloc_pt_region(mm_space_t *m_space)
{
    for (int i = 0; i < 8; i++)
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
    mpu_calc_regs(ri, addr, ffs(size), attrs, 0);
    return TRUE;
}
void mm_space_del(mm_space_t *m_space)
{
    /*TODO:*/
}
