#pragma once

#include <types.h>
#include "pager.h"
#include "asm/mm.h"
#include "asm_config.h"
#include <mm_page.h>
#include <vma.h>

typedef void *(*page_alloc_fn)(void);

static inline umword_t vpage_attrs_to_page_attrs(enum vpage_prot_attrs attrs)
{
    umword_t to_attrs = 0;

    to_attrs = PTE_SHARED | PTE_TYPE_PAGE | PTE_ATTRINDX(MT_NORMAL) | PTE_AF;

    if (attrs & VPAGE_PROT_UNCACHE)
    {
        /*TODO:设置无缓存属性*/
    }
    if (!(attrs & VPAGE_PROT_IN_KNL))
    {
        to_attrs |= PTE_NG;
    }
    if (attrs & VPAGE_PROT_RO)
    {
        to_attrs |= PTE_RDONLY;
    }
    if (attrs & VPAGE_PROT_WO)
    {
        to_attrs |= PTE_WONLY;
    }
    if (!(attrs & VPAGE_PROT_X))
    {
        to_attrs |= PTE_UXN;
    }
    if (!(attrs & VPAGE_PROT_UNCACHE))
    {
        to_attrs |= (0x1 << 4) | (0x3 << 2);
    }

    return to_attrs;
}

pte_t *pages_walk(page_entry_t *pdir, addr_t virt_addr, mword_t size, void *(*fn_alloc)(void));
void mmu_page_alloc_set(page_alloc_fn fn);
int map_mm(page_entry_t *pdir, addr_t virt_addr, addr_t phys_addr,
           mword_t page_order, mword_t pfn_cn, mword_t attr);
int unmap_mm(page_entry_t *pdir, addr_t virt_addr, mword_t page_order, mword_t pfn_cn);
umword_t mm_get_paddr(page_entry_t *pdir, addr_t virt_addr, mword_t page_order);
void per_cpu_boot_mapping(bool_t init_pages);
page_entry_t *boot_get_pdir(void);
void knl_pdir_init(page_entry_t *pdir, pte_t *dir, int page_deep);
