
#include <arch.h>
#include "asm/sysregs.h"
#include "asm/base.h"
#include "asm/mm.h"
#include <types.h>
#include <arch.h>
#include "pager.h"
#include <config.h>
#include <spinlock.h>
#include <assert.h>
#include <mm_page.h>
#include <mmu.h>
#define DATA_BOOT_SECTION ".data.boot"
#define TEXT_BOOT_SECTION ".text.boot"

#define TCR_DEFAULT (((1UL) << 31) | (1UL << 23) | \
                     (3UL << 12) | (1UL << 10) | (1UL << 8) | ((64UL - CONFIG_ARM64_VA_BITS)))

extern char _text_boot[];
extern char _etext_boot[];
extern char _data_boot[];
extern char _edata_boot[];
extern char _ebss[];
extern char _text[];
extern char _buddy_data_start[];

#define BOOT_PAGER_NR 64
static SECTION(DATA_BOOT_SECTION) __ALIGN__(PAGE_SIZE) uint8_t pages[BOOT_PAGER_NR * SYS_CPU_NUM][PAGE_SIZE];
static SECTION(DATA_BOOT_SECTION) uint8_t pages_used[BOOT_PAGER_NR * SYS_CPU_NUM];
static SECTION(TEXT_BOOT_SECTION) inline int boot_get_current_cpu_id(void)
{
    return read_sysreg(mpidr_el1) & 0XFFUL;
}
/**
 * 启动时使用这个分配内存，buddy初始化完成后，使用buddy分配内存
 */
static SECTION(TEXT_BOOT_SECTION) void *page_alloc(void)
{
    for (int i = 0; i < BOOT_PAGER_NR * SYS_CPU_NUM; i++)
    {
        if (pages_used[i + boot_get_current_cpu_id() * BOOT_PAGER_NR] == 0)
        {
            pages_used[i + boot_get_current_cpu_id() * BOOT_PAGER_NR] = 1;
            return (void *)pages[i + boot_get_current_cpu_id() * BOOT_PAGER_NR];
        }
    }
    return NULL;
}
static SECTION(TEXT_BOOT_SECTION) void page_free(void *mem)
{
    for (int i = 0; i < BOOT_PAGER_NR * SYS_CPU_NUM; i++)
    {
        if (pages_used[i + arch_get_current_cpu_id() * BOOT_PAGER_NR] == 1 && pages[i + arch_get_current_cpu_id() * BOOT_PAGER_NR] == mem)
        {
            pages_used[i + arch_get_current_cpu_id() * BOOT_PAGER_NR] = 0;
            break;
        }
    }
}
static page_alloc_fn page_alloc_cb = page_alloc;
SECTION(TEXT_BOOT_SECTION)
void mmu_page_alloc_set(page_alloc_fn fn)
{
    page_alloc_cb = fn;
}
static SECTION(TEXT_BOOT_SECTION) void *boot_memset(void *dst, int s, size_t count)
{
    register char *a = dst;
    count++; /* this actually creates smaller code than using count-- */
    while (--count)
        *a++ = s;
    return dst;
}

#define PTE_TYPE_BLOCK 0x0UL
#define PTE_TYPE_TAB 0x3UL

static SECTION(DATA_BOOT_SECTION) pte_t boot_kpdir[PAGE_SIZE / MWORD_BYTES];
SECTION(DATA_BOOT_SECTION)
static page_entry_t kpdir = {
    .dir = boot_kpdir,
    .depth = PAGE_DEEP,
    .lv_shift_sizes[0] = 39,
    .lv_shift_sizes[1] = 30,
    .lv_shift_sizes[2] = 21,
    .lv_shift_sizes[3] = 12};

void knl_pdir_init(page_entry_t *pdir, pte_t *dir, int page_deep)
{
    pdir->dir = dir;
    pdir->depth = page_deep;
    pdir->lv_shift_sizes[0] = 39;
    pdir->lv_shift_sizes[1] = 30;
    pdir->lv_shift_sizes[2] = 21;
    pdir->lv_shift_sizes[3] = 12;
}
SECTION(TEXT_BOOT_SECTION)
pte_t *pages_walk(page_entry_t *pdir, addr_t virt_addr, mword_t order, void *(*fn_alloc)(void))
{
    int i;
    pte_t *next = &pdir->dir[(virt_addr >> pdir->lv_shift_sizes[(PAGE_DEEP - pdir->depth)]) & 0x1ffUL];

    // 找到所在深度
    for (i = (PAGE_DEEP - pdir->depth); i < PAGE_DEEP; i++)
    {
        if (pdir->lv_shift_sizes[i] == order)
        {
            break;
        }
    }
    assert(i != PAGE_DEEP);

    // 向下循环查找，遇到没有分配的页表，则分配
    for (int j = (PAGE_DEEP - pdir->depth); j < PAGE_DEEP; j++)
    {
        if (j == i)
        {
            return next;
        }
        if (next->pte == 0)
        {
            if (!fn_alloc)
            {
                return NULL;
            }
            else
            {
                next->pte = (mword_t)fn_alloc();
                if (!(next->pte))
                {
                    return NULL;
                }
                next->pte |= 3UL;
                _dmb(ishst);
            }
        }
        assert((j + 1) < PAGE_DEEP);
        next = &((pte_t *)(next->pte & ~3UL))[(virt_addr >> pdir->lv_shift_sizes[j + 1]) & 0x1ffUL];
    }
    assert(0);
}
SECTION(TEXT_BOOT_SECTION)
int map_mm(page_entry_t *pdir, addr_t virt_addr, addr_t phys_addr,
           mword_t page_order, mword_t pfn_cn, mword_t attr)
{
    for (mword_t i = 0; i < pfn_cn; i++)
    {
        pte_t *pte = pages_walk(pdir, virt_addr + (i << page_order), page_order, page_alloc_cb);

        if (pte == NULL)
        {
            return -ENOMEM;
        }
        pte->pte = (phys_addr + (i << page_order)) | attr;
        _dmb(ishst);
    }
    return 0;
}
SECTION(TEXT_BOOT_SECTION)
int unmap_mm(page_entry_t *pdir, addr_t virt_addr, mword_t page_order, mword_t pfn_cn)
{
    for (mword_t i = 0; i < pfn_cn; i++)
    {
        pte_t *pte = pages_walk(pdir, virt_addr + (i << page_order), page_order, NULL);

        if (pte != NULL)
        {
            pte->pte = 0;
            _dmb(ishst);
        }
    }
    return 0;
}

static SECTION(TEXT_BOOT_SECTION) void boot_init_pageing(page_entry_t *kpdir, bool_t init_pages)
{
    uint64_t parang;
    uint64_t tmp;

    write_sysreg(0x00ff4400, mair_el2);
    if (init_pages)
    {
        int i_ffs = ffs(CONFIG_KNL_DATA_SIZE) + (is_power_of_2(CONFIG_KNL_DATA_SIZE) ? 0 : 1);
        int i_cn = 0;

        if (i_ffs<30) {
            i_ffs = 30;
        }

        // 进行1比1映射
        do
        {
            map_mm(kpdir, CONFIG_KNL_DATA_ADDR + ((1U << 30) * i_cn),
                   CONFIG_KNL_DATA_ADDR + ((1U << 30) * i_cn),
                   i_ffs > 30 ? 30 : i_ffs, 1, 0x709);
            i_ffs--;
            i_cn++;
        } while (i_ffs >= 30);
        // 外设地址也是1比1映射
        map_mm(kpdir, PBASE, PBASE, 21, DEVICE_SIZE >> 21, 0x709);
    }
    tmp = read_sysreg(ID_AA64MMFR0_EL1);
    parang = tmp & 0xf;
    if (parang > ID_AA64MMFR0_PARANGE_48)
    {
        parang = ID_AA64MMFR0_PARANGE_48;
    }
    write_sysreg(TCR_DEFAULT | (parang << 16UL), tcr_el2);
    _dsb(sy);
    write_sysreg(kpdir->dir, ttbr0_el2);
}

SECTION(TEXT_BOOT_SECTION)
int boot_enable_mmu(void)
{
    mword_t tmp;

    _dsb(ish);
    asm volatile("tlbi alle2is");
    asm volatile("tlbi vmalle1is");
    _dsb(ish);
    // write_sysreg(SCTLR_ELx_M, sctlr_el2);
    write_sysreg(0x30c51835, sctlr_el2);
    _isb();
    asm volatile("ic iallu");
    _dsb(nsh);
    _isb();
    return 0;
}

SECTION(TEXT_BOOT_SECTION)
page_entry_t *boot_get_pdir(void)
{
    return &kpdir;
}

SECTION(TEXT_BOOT_SECTION)
void per_cpu_boot_mapping(bool_t init_pages)
{
    boot_init_pageing(&kpdir, init_pages);
    _dsb(ish);
    asm volatile("ic iallu");
    boot_enable_mmu();
}
extern void jump_kenel_main(void);

// 启动的恒等映射
SECTION(TEXT_BOOT_SECTION)
void boot_mapping(void)
{
    boot_memset(pages, 0, sizeof(pages));
    boot_memset(pages_used, 0, sizeof(pages_used));
    per_cpu_boot_mapping(TRUE);
    jump_kenel_main();
}
