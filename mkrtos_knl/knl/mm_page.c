#include "types.h"
#include "mm_space.h"
#include "mm_wrap.h"
#include "assert.h"
#include "err.h"
#include "mm_page.h"
#include "mm_space.h"
#include "mpu.h"
static mm_entry_t *mm_pages_entry_alloc(mm_pages_t *mm, addr_t new_addr)
{
    for (int i = 0; i < PAGE_NR; i++)
    {
        if (mm->list[i].addr == 0)
        {
            mm_entry_set_addr(&mm->list[i].addr, new_addr);
            return &mm->list[i];
        }
    }
    return NULL;
}
static void mm_pages_entry_free(mm_pages_t *mm, addr_t free_addr)
{
    for (int i = 0; i < PAGE_NR; i++)
    {
        if (mm_entry_get_addr(mm->list[i].addr) == free_addr)
        {
            mm->list[i].addr = 0;
        }
    }
}
static mm_entry_t *mm_pages_find_first(mm_pages_t *mm, addr_t free_addr)
{
    for (int i = 0; i < PAGE_NR; i++)
    {
        if (mm_entry_get_addr(mm->list[i].addr) == free_addr && mm_entry_get_first(mm->list[i].addr))
        {
            return &mm->list[i];
        }
    }
    return NULL;
}
void mm_pages_init(mm_pages_t *mm, region_info_t *regi)
{
    for (int i = 0; i < PAGE_NR; i++)
    {
        mm->list[i].addr = 0;
    }
    mm->region = regi;
}
int mm_pages_alloc_page(mm_pages_t *mm, ram_limit_t *lim, size_t pnf_nr, addr_t *alloc_addr, uint8_t attrs)
{
    assert(mm);
    assert(alloc_addr);

    void *mem = mm_limit_alloc_align(lim, pnf_nr * PAGE_SIZE, PAGE_SIZE);
    if (!mem)
    {
        return -ENOMEM;
    }
    for (int i = 0; i < pnf_nr; i++)
    {
        mm_entry_t *mm_entry = mm_pages_entry_alloc(mm, (addr_t)mem + i * PAGE_SIZE);
        if (mm_entry == NULL)
        {
            mm_limit_free_align(lim, mem, pnf_nr * PAGE_SIZE);
            /*TODO:清除申请的所有*/
            return -ENOMEM;
        }
        if (i == 0)
        {
            mm_entry_set_first(&mm_entry->addr);
            mm_entry->attrs = attrs;
            mm_entry->pfn_nr = pnf_nr;
        }
        else
        {
            mm_entry->attrs = attrs;
            mm_entry->pfn_nr = 1;
        }
    }
    *alloc_addr = (addr_t)mem;
    return 0;
}
void mm_pages_free_page(mm_pages_t *mm, ram_limit_t *lim, addr_t addr, size_t pfn_nr)
{
    addr = ALIGN_DOWN(addr, PAGE_SIZE);
    mm_entry_t *mm_entry = mm_pages_find_first(mm, addr);

    if (!mm_entry)
    {
        return;
    }
    addr_t start_addr = mm_entry_get_addr(mm_entry->addr);

    mm_limit_free_align(lim, (void *)start_addr, PAGE_SIZE * mm_entry->pfn_nr);
    for (int i = 0; i < pfn_nr; i++)
    {
        mm_pages_entry_free(mm, start_addr);
        start_addr += PAGE_SIZE;
    }
    mpu_region_clr(mm->region->region_inx);
}

void *mm_page_alloc_fault(mm_pages_t *mm, addr_t addr)
{
    addr = ALIGN_DOWN(addr, PAGE_SIZE);
    for (int i = 0; i < PAGE_NR; i++)
    {
        if (mm_entry_get_addr(mm->list[i].addr) == addr)
        {
            mpu_calc_regs(mm->region, addr, ffs(PAGE_SIZE),
                          mm->list[i].attrs, 0x0);
            mpu_region_set(mm->region->region_inx, mm->region->rbar,
                           mm->region->rasr);
            return (void *)(mm->list[i].addr);
        }
    }

    return NULL;
}