#pragma once

#include <types.h>
#include <util.h>
typedef struct pgd
{
    mword_t pgd;
} pgd_t;

typedef struct pud
{
    mword_t pud;
} pud_t;

typedef struct pmd
{
    mword_t pmd;
} pmd_t;

typedef struct pte
{
    mword_t pte;
} pte_t;

#define PTE_GET_VALID(p) (p & 0x1UL)
#define PTE_GET_TYPE(p) (p & 0x2UL)

#define PTE_LOW_ATTR(p) (MASK_LSB(p, 2) & 0x3ffUL)
#define PTE_HIGH_ATTR(p) (MASK_LSB(p, 52) >> 52UL)

#define PTE_ADDR(p, n) (MASK_LSB(p, n))
#define PTE_PA(p, n) PTE_ADDR(p, n)
#define PTE_NEXT_BASE_ADDR(p, n) PTE_ADDR(p, n)

#define PTE_PA_4K(p) PTE_ADDR(p, 12)

/* PGD */
#define PGDIR_SHIFT 39
#define PGDIR_SIZE (1UL << PGDIR_SHIFT)
#define PGDIR_MASK (~(PGDIR_SIZE - 1))
#define PTRS_PER_PGD (1 << (VA_BITS - PGDIR_SHIFT))

/* PUD */
#define PUD_SHIFT 30
#define PUD_SIZE (1UL << PUD_SHIFT)
#define PUD_MASK (~(PUD_SIZE - 1))
#define PTRS_PER_PUD (1 << (PGDIR_SHIFT - PUD_SHIFT))

/* PMD */
#define PMD_SHIFT 21
#define PMD_SIZE (1UL << PMD_SHIFT)
#define PMD_MASK (~(PMD_SIZE - 1))
#define PTRS_PER_PMD (1 << (PUD_SHIFT - PMD_SHIFT))

/* PTE */
#define PTE_SHIFT 12
#define PTE_SIZE (1UL << PTE_SHIFT)
#define PTE_MASK (~(PTE_SIZE - 1))
#define PTRS_PER_PTE (1 << (PMD_SHIFT - PTE_SHIFT))

/* Section */
#define SECTION_SHIFT PMD_SHIFT
#define SECTION_SIZE (1UL << SECTION_SHIFT)
#define SECTION_MASK (~(SECTION_SIZE - 1))

// Level 3 descriptor PTE高位属性
#define PTE_TYPE_MASK (3UL << 0)
#define PTE_TYPE_FAULT (0UL << 0)
#define PTE_TYPE_PAGE (3UL << 0)
#define PTE_TABLE_BIT (1UL << 1)
#define PTE_USER (1UL << 6)    /* AP[1] */
#define PTE_RDONLY (1UL << 7)  /* AP[2] */
#define PTE_SHARED (3UL << 8)  /* SH[1:0], inner shareable */
#define PTE_AF (1UL << 10)     /* Access Flag */
#define PTE_NG (1UL << 11)     /* nG */
#define PTE_DBM (1UL << 51)    /* Dirty Bit Management */
#define PTE_CONT (1UL << 52)   /* Contiguous range */
#define PTE_PXN (1UL << 53)    /* Privileged XN */
#define PTE_UXN (1UL << 54)    /* User XN */
#define PTE_HYP_XN (1UL << 54) /* HYP XN */

/*
 * AttrIndx[2:0] encoding
 * (mapping attributes defined in the MAIR* registers).
 */
#define PTE_ATTRINDX(t) ((t) << 2)
#define PTE_ATTRINDX_MASK (7 << 2)

// #define TCR_IPS_MASK (0x7UL << 32)
// #define TCR_TG1_MASK (0x3UL << 30)
// #define TCR_T1SZ_MASK (0x3fUL << 16)
// #define TCR_TG0_MASK (0x3UL << 14)
// #define TCR_T0SZ_MASK (0x3fUL << 0)

#define SCTLR_M_MASK (0x1UL << 0)
#define SCTLR_I_MASK (0x1UL << 12)
#define SCTRL_C_MASK (0x1UL << 2)

/*
 * Memory types available.
 */
#define MT_DEVICE_nGnRnE 0
#define MT_DEVICE_nGnRE 1
#define MT_DEVICE_GRE 2
#define MT_NORMAL_NC 3
#define MT_NORMAL 4
#define MT_NORMAL_WT 5

/*
 * Software defined PTE bits definition.
 */
#define PTE_VALID (1UL << 0)
#define PTE_WRITE (PTE_DBM) /* same as DBM (51) */
#define PTE_DIRTY (1UL << 55)
#define PTE_SPECIAL (1UL << 56)
#define PTE_PROT_NONE (1UL << 58) /* only when !PTE_VALID */

#define MAIR(attr, mt) ((attr) << ((mt) * 8))

#define _PROT_DEFAULT (PTE_TYPE_PAGE | PTE_AF | PTE_SHARED)
#define PROT_DEFAULT (_PROT_DEFAULT)

#define PAGE_KERNEL_RO ((PROT_NORMAL & ~PTE_WRITE) | PTE_RDONLY)
#define PAGE_KERNEL_ROX ((PROT_NORMAL & ~(PTE_WRITE | PTE_PXN)) | PTE_RDONLY)
#define PAGE_KERNEL_EXEC (PROT_NORMAL & ~PTE_PXN)

#define PROT_DEVICE_nGnRnE (PROT_DEFAULT | PTE_PXN | PTE_UXN | PTE_DIRTY | PTE_WRITE | PTE_ATTRINDX(MT_DEVICE_nGnRnE))
#define PROT_DEVICE_nGnRE (PROT_DEFAULT | PTE_PXN | PTE_UXN | PTE_DIRTY | PTE_WRITE | PTE_ATTRINDX(MT_DEVICE_nGnRE))
#define PROT_NORMAL_NC (PROT_DEFAULT | PTE_PXN | PTE_UXN | PTE_DIRTY | PTE_WRITE | PTE_ATTRINDX(MT_NORMAL_NC))
#define PROT_NORMAL_WT (PROT_DEFAULT | PTE_PXN | PTE_UXN | PTE_DIRTY | PTE_WRITE | PTE_ATTRINDX(MT_NORMAL_WT))
#define PROT_NORMAL (PROT_DEFAULT | PTE_PXN | PTE_UXN | PTE_DIRTY | PTE_WRITE | PTE_ATTRINDX(MT_NORMAL))

#define PAGE_KERNEL PROT_NORMAL

/*
 * TCR flags.
 */
#define TCR_T0SZ_OFFSET 0
#define TCR_T1SZ_OFFSET 16
#define TCR_T0SZ(x) ((UL(64) - (x)) << TCR_T0SZ_OFFSET)
#define TCR_T1SZ(x) ((UL(64) - (x)) << TCR_T1SZ_OFFSET)
#define TCR_TxSZ(x) (TCR_T0SZ(x) | TCR_T1SZ(x))
#define TCR_TxSZ_WIDTH 6
// #define TCR_T0SZ_MASK		(((UL(1) << TCR_TxSZ_WIDTH) - 1) << TCR_T0SZ_OFFSET)

#define TCR_EPD0_SHIFT 7
#define TCR_EPD0_MASK (UL(1) << TCR_EPD0_SHIFT)
#define TCR_IRGN0_SHIFT 8
#define TCR_IRGN0_MASK (UL(3) << TCR_IRGN0_SHIFT)
#define TCR_IRGN0_NC (UL(0) << TCR_IRGN0_SHIFT)
#define TCR_IRGN0_WBWA (UL(1) << TCR_IRGN0_SHIFT)
#define TCR_IRGN0_WT (UL(2) << TCR_IRGN0_SHIFT)
#define TCR_IRGN0_WBnWA (UL(3) << TCR_IRGN0_SHIFT)

#define TCR_EPD1_SHIFT 23
#define TCR_EPD1_MASK (UL(1) << TCR_EPD1_SHIFT)
#define TCR_IRGN1_SHIFT 24
#define TCR_IRGN1_MASK (UL(3) << TCR_IRGN1_SHIFT)
#define TCR_IRGN1_NC (UL(0) << TCR_IRGN1_SHIFT)
#define TCR_IRGN1_WBWA (UL(1) << TCR_IRGN1_SHIFT)
#define TCR_IRGN1_WT (UL(2) << TCR_IRGN1_SHIFT)
#define TCR_IRGN1_WBnWA (UL(3) << TCR_IRGN1_SHIFT)

#define TCR_IRGN_NC (TCR_IRGN0_NC | TCR_IRGN1_NC)
#define TCR_IRGN_WBWA (TCR_IRGN0_WBWA | TCR_IRGN1_WBWA)
#define TCR_IRGN_WT (TCR_IRGN0_WT | TCR_IRGN1_WT)
#define TCR_IRGN_WBnWA (TCR_IRGN0_WBnWA | TCR_IRGN1_WBnWA)
#define TCR_IRGN_MASK (TCR_IRGN0_MASK | TCR_IRGN1_MASK)

#define TCR_ORGN0_SHIFT 10
#define TCR_ORGN0_MASK (UL(3) << TCR_ORGN0_SHIFT)
#define TCR_ORGN0_NC (UL(0) << TCR_ORGN0_SHIFT)
#define TCR_ORGN0_WBWA (UL(1) << TCR_ORGN0_SHIFT)
#define TCR_ORGN0_WT (UL(2) << TCR_ORGN0_SHIFT)
#define TCR_ORGN0_WBnWA (UL(3) << TCR_ORGN0_SHIFT)

#define TCR_ORGN1_SHIFT 26
#define TCR_ORGN1_MASK (UL(3) << TCR_ORGN1_SHIFT)
#define TCR_ORGN1_NC (UL(0) << TCR_ORGN1_SHIFT)
#define TCR_ORGN1_WBWA (UL(1) << TCR_ORGN1_SHIFT)
#define TCR_ORGN1_WT (UL(2) << TCR_ORGN1_SHIFT)
#define TCR_ORGN1_WBnWA (UL(3) << TCR_ORGN1_SHIFT)

#define TCR_ORGN_NC (TCR_ORGN0_NC | TCR_ORGN1_NC)
#define TCR_ORGN_WBWA (TCR_ORGN0_WBWA | TCR_ORGN1_WBWA)
#define TCR_ORGN_WT (TCR_ORGN0_WT | TCR_ORGN1_WT)
#define TCR_ORGN_WBnWA (TCR_ORGN0_WBnWA | TCR_ORGN1_WBnWA)
#define TCR_ORGN_MASK (TCR_ORGN0_MASK | TCR_ORGN1_MASK)

#define TCR_SH0_SHIFT 12
#define TCR_SH0_MASK (UL(3) << TCR_SH0_SHIFT)
#define TCR_SH0_INNER (UL(3) << TCR_SH0_SHIFT)

#define TCR_SH1_SHIFT 28
#define TCR_SH1_MASK (UL(3) << TCR_SH1_SHIFT)
#define TCR_SH1_INNER (UL(3) << TCR_SH1_SHIFT)
#define TCR_SHARED (TCR_SH0_INNER | TCR_SH1_INNER)

#define TCR_TG0_SHIFT 14
#define TCR_TG0_MASK (UL(3) << TCR_TG0_SHIFT)
#define TCR_TG0_4K (UL(0) << TCR_TG0_SHIFT)
#define TCR_TG0_64K (UL(1) << TCR_TG0_SHIFT)
#define TCR_TG0_16K (UL(2) << TCR_TG0_SHIFT)

#define TCR_TG1_SHIFT 30
#define TCR_TG1_MASK (UL(3) << TCR_TG1_SHIFT)
#define TCR_TG1_16K (UL(1) << TCR_TG1_SHIFT)
#define TCR_TG1_4K (UL(2) << TCR_TG1_SHIFT)
#define TCR_TG1_64K (UL(3) << TCR_TG1_SHIFT)

#define TCR_IPS_SHIFT 32
#define TCR_IPS_MASK (UL(7) << TCR_IPS_SHIFT)
#define TCR_A1 (UL(1) << 22)
#define TCR_ASID16 (UL(1) << 36)
#define TCR_TBI0 (UL(1) << 37)
#define TCR_TBI1 (UL(1) << 38)
#define TCR_HA (UL(1) << 39)
#define TCR_HD (UL(1) << 40)
#define TCR_NFD1 (UL(1) << 54)

#define TCR_TG_FLAGS (TCR_TG0_4K | TCR_TG1_4K)
#define TCR_KASLR_FLAGS 0
#define TCR_KASAN_FLAGS 0
#define TCR_SMP_FLAGS TCR_SHARED
#define TCR_CACHE_FLAGS (TCR_IRGN_WBWA | TCR_ORGN_WBWA)

#define TCR_48BITS (((64 - VA_BITS) << 0) | ((64 - VA_BITS) << 16))
#define TCR_IPS_48BITS (ID_AA64MMFR0_PARANGE_48 << 32)

/* id_aa64mmfr0 */
#define ID_AA64MMFR0_TGRAN4_SHIFT 28
#define ID_AA64MMFR0_TGRAN64_SHIFT 24
#define ID_AA64MMFR0_TGRAN16_SHIFT 20
#define ID_AA64MMFR0_BIGENDEL0_SHIFT 16
#define ID_AA64MMFR0_SNSMEM_SHIFT 12
#define ID_AA64MMFR0_BIGENDEL_SHIFT 8
#define ID_AA64MMFR0_ASID_SHIFT 4
#define ID_AA64MMFR0_PARANGE_SHIFT 0

#define ID_AA64MMFR0_TGRAN4_NI 0xf
#define ID_AA64MMFR0_TGRAN4_SUPPORTED 0x0
#define ID_AA64MMFR0_TGRAN64_NI 0xf
#define ID_AA64MMFR0_TGRAN64_SUPPORTED 0x0
#define ID_AA64MMFR0_TGRAN16_NI 0x0
#define ID_AA64MMFR0_TGRAN16_SUPPORTED 0x1
#define ID_AA64MMFR0_PARANGE_48 0x5
#define ID_AA64MMFR0_PARANGE_52 0x6

#if defined(CONFIG_ARM64_4K_PAGES)
#define ID_AA64MMFR0_TGRAN_SHIFT ID_AA64MMFR0_TGRAN4_SHIFT
#define ID_AA64MMFR0_TGRAN_SUPPORTED ID_AA64MMFR0_TGRAN4_SUPPORTED
#endif

#define PTE_ADDR_LOW (((1UL << (48 - PAGE_SHIFT)) - 1) << PAGE_SHIFT)
#define PTE_ADDR_MASK PTE_ADDR_LOW

void paging_init(void);
