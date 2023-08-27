#pragma once

#include "types.h"
#include "ram_limit.h"
#include "util.h"

typedef addr_t mm_addr_t;
struct region_info;
typedef struct region_info region_info_t;

// 使用一个mpu区域模拟页表
// 该文件实现内存的管理
typedef struct mm_entry
{
    mm_addr_t addr;  //!< 页表的地址
    uint16_t pfn_nr; //!< 多少页
    uint8_t attrs;   //!< 属性
} mm_entry_t;

static inline void mm_entry_set_first(mm_addr_t *addr)
{
    *addr |= 1UL;
}
static inline bool_t mm_entry_get_first(mm_addr_t addr)
{
    return addr & 0x1UL;
}
static inline addr_t mm_entry_get_addr(mm_addr_t addr)
{
    return MASK_LSB(addr, 2);
}
static inline void mm_entry_set_addr(mm_addr_t *addr, addr_t new_addr)
{
    *addr &= ~3UL;
    *addr |= new_addr & (~3UL);
}

#define PAGE_MAN_NR 32

#define PAGE_NR 64
#define PAGE_SIZE 512

typedef struct mm_pages
{
    mm_entry_t list[PAGE_NR]; // TODO: 使用hashmap
    region_info_t *region;    //!< pages使用的mpu region
} mm_pages_t;

void mm_pages_init(mm_pages_t *mm, region_info_t *regi);
int mm_pages_alloc_page(mm_pages_t *mm, ram_limit_t *lim, size_t pnf_nr, addr_t *alloc_addr, uint8_t attrs);
void mm_pages_free_page(mm_pages_t *mm, ram_limit_t *lim, addr_t addr, size_t pfn_nr);
