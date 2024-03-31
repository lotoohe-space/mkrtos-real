
#include <types.h>
// #include <mkrtos.h>
#include "asm/sysregs.h"
#include <arch.h>
#include "asm/base.h"
#include "pager.h"
#include "asm/mm.h"
#include <printk.h>
static inline mword_t pgd_offset(addr_t virt)
{
    return (((virt) >> PGDIR_SHIFT) & (PTRS_PER_PGD - 1));
}
static inline mword_t pmd_offset(addr_t virt)
{
    return (((virt) >> PMD_SHIFT) & (PTRS_PER_PMD - 1));
}
static inline mword_t pud_offset(addr_t virt)
{
    return (((virt) >> PUD_SHIFT) & (PTRS_PER_PUD - 1));
}
static inline mword_t pte_offset(addr_t virt)
{
    return (((virt) >> PAGE_SHIFT) & (PTRS_PER_PTE - 1));
}

static void pte_print(pgd_t *pgdp)
{
    for (size_t i = 0; i < PAGE_SIZE / sizeof(void *); i++)
    {
        if (PTE_GET_VALID(pgdp[i].pgd))
        {
            printk("%d pgd 0x%lx, next pg 0x%lx\n", i, pgdp[i].pgd, PTE_ADDR(pgdp[i].pgd, PAGE_SHIFT));
            for (size_t j = 0; j < PAGE_SIZE / sizeof(void *); j++)
            {
                pud_t *pudp = &((pud_t *)(PTE_ADDR(pgdp[i].pgd, PAGE_SHIFT)))[j];
                if (PTE_GET_VALID(pudp->pud))
                {
                    printk(" %d pud 0x%lx, next pg 0x%lx\n", j, pudp->pud, PTE_ADDR(pudp->pud, PAGE_SHIFT));
                    for (size_t m = 0; m < PAGE_SIZE / sizeof(void *); m++)
                    {
                        pmd_t *pmdp = &((pmd_t *)(PTE_ADDR(pudp->pud, PAGE_SHIFT)))[m];
                        if (PTE_GET_VALID(pmdp->pmd) && (pmdp->pmd & 0x2))
                        {
                            printk("  %d pmd 0x%lx, next pg 0x%lx\n", m, pmdp->pmd, PTE_ADDR(pmdp->pmd, PAGE_SHIFT));

                            for (size_t n = 0; n < PAGE_SIZE / sizeof(void *); n++)
                            {
                                pte_t *ptep = &((pte_t *)(PTE_ADDR(pmdp->pmd, PAGE_SHIFT)))[n];
                                if (PTE_GET_VALID(ptep->pte))
                                {
                                    printk("  %d pte 0x%lx, next pg 0x%lx\n", n, ptep->pte, PTE_ADDR(ptep->pte, PAGE_SHIFT));
                                }
                            }
                        }
                        else if (PTE_GET_VALID(pmdp->pmd))
                        {
                            printk("  %d pmd 0x%lx, addr 0x%lx\n", m, pmdp->pmd, PTE_ADDR(pmdp->pmd, PAGE_SHIFT));
                        }
                    }
                }
            }
        }
    }
}
