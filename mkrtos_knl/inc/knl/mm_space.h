#pragma once

#include "types.h"
// #include <mm_page.h>
#include <assert.h>
#include <util.h>
#include <arch.h>
#include <vma.h>
/**
 * @brief 内存空间
 *
 */
typedef struct mm_space
{
#if IS_ENABLED(CONFIG_MMU)
    page_entry_t mem_dir; //!< MMU根映射表，存放映射信息
    umword_t asid;        //!< 进程的ASID
#else
    void *mm_block;       //!< task 的私有内存块
    size_t mm_block_size; //!< 私有内存块的大小
#endif
    task_vma_t mem_vma; //!< 虚拟内存区域
} mm_space_t;

enum region_rights
{
    REGION_PRIV = 1,
    REGION_RO = 2,
    REGION_RWX = 3,
};

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
static inline page_entry_t *mm_space_get_pdir(mm_space_t *sp)
{
    return &sp->mem_dir;
}
#endif
int mm_space_init(mm_space_t *mm_space, int is_knl);
