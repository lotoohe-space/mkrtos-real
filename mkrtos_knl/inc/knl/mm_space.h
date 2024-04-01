#pragma once

#include "types.h"
#include "mm_page.h"
#include <assert.h>
#include <util.h>
#if CONFIG_MK_MPU_CFG
#include <early_boot.h>
#endif
#if !IS_ENABLED(CONFIG_MMU)
typedef struct region_info
{
#if CONFIG_MK_MPU_CFG
    umword_t start_addr;       //!< 内存申请的开始地址
    umword_t block_start_addr; //!< 块申请的开始地址
    umword_t block_size;       //!< 保护的块大小
    umword_t size;             //!< 实际申请的内存大小
    umword_t rbar;             //!< mpu保护寄存器信息
    umword_t rasr;             //!< mpu保护寄存器信息
    int16_t region_inx;
    uint8_t region; //!< 区域禁止信息
#endif
} region_info_t;
#endif
typedef struct mm_space
{
#if CONFIG_MK_MPU_CFG
    region_info_t pt_regions[CONFIG_REGION_NUM]; //!< mpu内存保护块
#endif
#if IS_ENABLED(CONFIG_MMU)
    page_entry_t mem_dir; //!< MMU根映射表，存放映射信息
#endif
    void *mm_block;       //!< task 的私有内存块
    size_t mm_block_size; //!< 私有内存块的大小
} mm_space_t;

static inline page_entry_t *mm_space_get_pdir(mm_space_t *sp)
{
    return &sp->mem_dir;
}

enum region_rights
{
    REGION_PRIV = 1,
    REGION_RO = 2,
    REGION_RWX = 3,
};
#if CONFIG_MK_MPU_CFG

region_info_t *mm_space_alloc_pt_region(mm_space_t *m_space);
void mm_space_free_pt_region(mm_space_t *m_space, region_info_t *ri);

void mm_space_init(mm_space_t *mm_space, int is_knl);
bool_t mm_space_add(mm_space_t *m_space, umword_t addr, umword_t size, uint8_t attrs);
void mm_space_del(mm_space_t *m_space, umword_t addr);
#else
#if !IS_ENABLED(CONFIG_MMU)
static inline void mm_space_init(mm_space_t *mm_space, int is_knl)
{
}
static inline bool_t mm_space_add(mm_space_t *m_space, umword_t addr, umword_t size, uint8_t attrs)
{
    return TRUE;
}
static inline void mm_space_del(mm_space_t *m_space, umword_t addr)
{
}
#else
void mm_space_init(mm_space_t *mm_space, int is_knl);
bool_t mm_space_add(mm_space_t *m_space, umword_t addr, umword_t size, uint8_t attrs);
void mm_space_del(mm_space_t *m_space, umword_t addr);
#endif
#endif

#if !IS_ENABLED(CONFIG_MMU)
static inline void mm_space_set_ram_block(mm_space_t *mm_space, void *mem, size_t size)
{
    mm_space->mm_block = mem;
    mm_space->mm_block_size = size;
}
static inline void mm_space_get_ram_block(mm_space_t *mm_space, void **mem, size_t *size)
{
    assert(mem);
    assert(size);
    *mem = mm_space->mm_block;
    *size = mm_space->mm_block_size;
}
#else
static inline void mm_space_set_ram_block(mm_space_t *mm_space, void *mem, size_t size)
{
}
static inline void mm_space_get_ram_block(mm_space_t *mm_space, void **mem, size_t *size)
{
}
#endif
