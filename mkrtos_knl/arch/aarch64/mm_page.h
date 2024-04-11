#pragma once

#include <types.h>
#include "pager.h"
#include <err.h>
#define PAGE_DEEP 4

typedef struct page_entry
{
    pte_t *dir;                        //!< 存储页表地址
    uint8_t lv_shift_sizes[PAGE_DEEP]; //!< 页表翻译的大小，order
    uint8_t depth;                     //!< 页表深度
} page_entry_t;

int page_entry_init(page_entry_t *entry);
