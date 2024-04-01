#pragma once

#include <types.h>
#include "pager.h"
#include "asm/mm.h"
#include "asm_config.h"
#include <mm_page.h>
pte_t *pages_walk(page_entry_t *pdir, addr_t virt_addr, mword_t size, void *(*fn_alloc)(void));
void map_mm(page_entry_t *pdir, addr_t virt_addr, addr_t phys_addr,
            mword_t page_order, mword_t pfn_cn, mword_t attr);
void per_cpu_boot_mapping(bool_t init_pages);
page_entry_t *boot_get_pdir(void);
void knl_pdir_init(page_entry_t *pdir, pte_t *dir, int page_deep);
