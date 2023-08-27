#pragma once

#include "types.h"
#include "mm_page.h"

#define REGION_NUM 8 //!< 默认为8
typedef struct region_info
{
    umword_t start_addr;       //!< 内存申请的开始地址
    umword_t block_start_addr; //!< 块申请的开始地址
    umword_t block_size;       //!< 保护的块大小
    umword_t size;             //!< 实际申请的内存大小
    umword_t rbar;             //!< mpu保护寄存器信息
    umword_t rasr;             //!< mpu保护寄存器信息
    int16_t region_inx;
    uint8_t region;            //!< 区域禁止信息
} region_info_t;

typedef struct mm_space
{
    region_info_t pt_regions[REGION_NUM];
    mm_pages_t mm_pages;
} mm_space_t;

enum region_rights
{
    REGION_PRIV = 1,
    REGION_RO = 2,
    REGION_RWX = 3,
};

region_info_t *mm_space_alloc_pt_region(mm_space_t *m_space);
void mm_space_free_pt_region(mm_space_t *m_space, region_info_t *ri);

void mm_space_init(mm_space_t *mm_space, int is_knl);
bool_t mm_space_add(mm_space_t *m_space, umword_t addr, umword_t size, uint8_t attrs);