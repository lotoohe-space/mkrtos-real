#pragma once

#include <arch.h>
#include <assert.h>
#include <printk.h>
#include <types.h>
#include <util.h>
#include <spinlock.h>
// #include <timer.h>
/*
 * ID0-ID15，分配给SGI (一般会将0-7给REE,8-15给TEE)
 * ID16-ID31，分配给PPI
 * ID32-ID1019，分配给SPI
 * ID1020-ID1023，特殊中断号
 * ID1024-ID8191，reserved
 * 8192及其以上，LPI
 */
typedef struct gic
{
    uint16_t irqs_number; //!< 支持的irq数量

    addr_t disp_base_addr;  //!< 分发器起始地址
    addr_t inter_base_addr; //!< 接口起始地址
    spinlock_t lock;
} gic_t;

#define GIC2_GICD_REG_BASE 0x08000000
#define GIC2_GICC_REG_BASE 0x08010000
// #define GIC2_BASE (0xFF840000)
// #define GIC2_GICD_BASE (GIC2_BASE + 0x1000)

#define GICD_CTLR(GIC2_GICD_BASE) (0x0 + GIC2_GICD_BASE)
#define GICD_TYPER(GIC2_GICD_BASE) (0x4 + GIC2_GICD_BASE)
#define GICD_IIDR(GIC2_GICD_BASE) (0x8 + GIC2_GICD_BASE)
#define GICD_IGROUPRn(GIC2_GICD_BASE) (0x80 + GIC2_GICD_BASE)
#define GICD_ISENABLERn(GIC2_GICD_BASE) (0x100 + GIC2_GICD_BASE)
#define GICD_ICENABLERn(GIC2_GICD_BASE) (0x180 + GIC2_GICD_BASE)
#define GICD_ISPENDRn(GIC2_GICD_BASE) (0x200 + GIC2_GICD_BASE)
#define GICD_ICPENDRn(GIC2_GICD_BASE) (0x280 + GIC2_GICD_BASE)
#define GICD_ISACTIVERn(GIC2_GICD_BASE) (0x300 + GIC2_GICD_BASE)
#define GICD_ICACTIVERn(GIC2_GICD_BASE) (0x380 + GIC2_GICD_BASE)
#define GICD_IPRIORITYRn(GIC2_GICD_BASE) (0x400 + GIC2_GICD_BASE)
#define GICD_ITARGETSRn(GIC2_GICD_BASE) (0x800 + GIC2_GICD_BASE)
#define GICD_ICFGRn(GIC2_GICD_BASE) (0xC00 + GIC2_GICD_BASE)
#define GICD_SGIR(GIC2_GICD_BASE) (0xf00 + GIC2_GICD_BASE)
#define GICD_CPENDSGIRn(GIC2_GICD_BASE) (0xf10 + GIC2_GICD_BASE)
#define GICD_SPENDSGIRn(GIC2_GICD_BASE) (0xf20 + GIC2_GICD_BASE)

// #define (GIC2_GICC_BASE) (GIC2_BASE + 0x2000)
#define GICC_CTLR(GIC2_GICC_BASE) (0x0 + GIC2_GICC_BASE)
#define GICC_PMR(GIC2_GICC_BASE) (0x4 + GIC2_GICC_BASE)
#define GICC_BPR(GIC2_GICC_BASE) (0x8 + GIC2_GICC_BASE)
#define GICC_IAR(GIC2_GICC_BASE) (0xc + GIC2_GICC_BASE)
#define GICC_EOIR(GIC2_GICC_BASE) (0x10 + GIC2_GICC_BASE)
#define GICC_RPR(GIC2_GICC_BASE) (0x14 + GIC2_GICC_BASE)
#define GICC_HPPIR(GIC2_GICC_BASE) (0x18 + GIC2_GICC_BASE)
#define GICC_APRn(GIC2_GICC_BASE) (0xd0 + GIC2_GICC_BASE)
#define GICC_IIDR(GIC2_GICC_BASE) (0x00FC + GIC2_GICC_BASE)

#define MAX_INTR_NO 1020

static inline void gic2_set_sgir(gic_t *irq, uint64_t cpu_mask)
{
    write_reg32(GICD_SGIR(irq->disp_base_addr),
                read_reg32(GICD_SGIR(irq->disp_base_addr)) |
                    ((cpu_mask & 0xff) << 16UL));
}
static inline void gic2_set_unmask(gic_t *irq, uint64_t inx)
{
    assert(inx < MAX_INTR_NO);
    void *addr = (void *)(GICD_ISENABLERn(irq->disp_base_addr) + ((inx >> 5) << 2));
    uint32_t tmp = read_reg32(addr);
    uint32_t bit_inx = inx % 32;

    MK_CLR_BIT(tmp, bit_inx);
    tmp |= (1) << bit_inx;
    write_reg32((uint32_t *)addr, tmp);
}
static inline void gic2_set_mask(gic_t *irq, uint64_t inx)
{
    assert(inx < MAX_INTR_NO);
    void *addr = (void *)(GICD_ICENABLERn(irq->disp_base_addr) + ((inx >> 5) << 2));
    uint32_t tmp = read_reg32(addr);
    uint32_t bit_inx = inx % 32;

    MK_CLR_BIT(tmp, bit_inx);
    tmp |= (1) << bit_inx;
    write_reg32((uint32_t *)addr, tmp);
}
static inline bool_t gic2_get_mask(gic_t *irq, uint64_t inx)
{
    assert(inx < MAX_INTR_NO);
    void *addr = (void *)(GICD_ISENABLERn(irq->disp_base_addr) +
                          ((inx >> 5) << 2));
    uint32_t tmp = read_reg32((uint32_t *)addr);
    uint32_t bit_inx = inx % 32;

    return !!((tmp >> bit_inx) & 0x1ul);
}
static inline void gic2_set_active(gic_t *irq, uint64_t inx)
{
    assert(inx < MAX_INTR_NO);
    void *addr = (void *)(GICD_ISACTIVERn(irq->disp_base_addr) + ((inx >> 5) << 2));
    uint32_t tmp = read_reg32(addr);
    uint32_t bit_inx = inx % 32;

    MK_CLR_BIT(tmp, bit_inx);
    tmp |= (1) << bit_inx;
    write_reg32((uint32_t *)addr, tmp);
}
static inline void gic2_clear_active(gic_t *irq, uint64_t inx)
{
    assert(inx < MAX_INTR_NO);
    void *addr = (void *)(GICD_ICACTIVERn(irq->disp_base_addr) + ((inx >> 5) << 2));
    uint32_t tmp = read_reg32(addr);
    uint32_t bit_inx = inx % 32;

    MK_CLR_BIT(tmp, bit_inx);
    tmp |= (1) << bit_inx;
    write_reg32((uint32_t *)addr, tmp);
}
static inline void gic2_sgi_set_pending(gic_t *irq, uint64_t inx, uint8_t cpu_mask)
{
    assert(inx < 16);
    void *addr = (void *)(GICD_SPENDSGIRn(irq->disp_base_addr) + (inx >> 2));
    uint32_t tmp = read_reg32(addr);
    uint32_t bit_inx = (inx % 4) << 3;

    tmp &= (~0xff) << bit_inx;
    tmp |= cpu_mask << bit_inx;
    write_reg32((uint32_t *)addr, tmp);
}
static inline void gic2_set_pending(gic_t *irq, uint64_t inx)
{
    assert(inx < MAX_INTR_NO);
    void *addr = (void *)(GICD_ISPENDRn(irq->disp_base_addr) + ((inx >> 5) << 2));
    uint32_t tmp = read_reg32(addr);
    uint32_t bit_inx = inx % 32;

    MK_CLR_BIT(tmp, bit_inx);
    tmp |= (1) << bit_inx;
    write_reg32((uint32_t *)addr, tmp);
}
static inline void gic2_clear_pending(gic_t *irq, uint64_t inx)
{
    assert(inx < MAX_INTR_NO);
    void *addr = (void *)(GICD_ICPENDRn(irq->disp_base_addr) + ((inx >> 5) << 2));
    uint32_t tmp = read_reg32(addr);
    uint32_t bit_inx = inx % 32;

    MK_CLR_BIT(tmp, bit_inx);
    tmp |= (1) << bit_inx;
    write_reg32((uint32_t *)addr, tmp);
}

static inline void gic2_set_target_cpu(gic_t *irq, uint64_t inx, uint64_t target_cpu)
{
    assert(inx < MAX_INTR_NO);
    // assert(target_cpu < 8);
    void *addr = (void *)(GICD_ITARGETSRn((irq->disp_base_addr)) +
                          ((inx >> 2) << 2));
    uint32_t bit_inx = inx % 4;
    uint32_t tmp = read_reg32((uint32_t *)addr);

    tmp &= ~(0xffUL << (bit_inx << 3));
    tmp |= target_cpu << (bit_inx << 3);
    write_reg32((uint32_t *)addr, tmp);
    tmp = read_reg32((uint32_t *)addr);
}
static inline uint64_t gic2_get_target_cpu(gic_t *irq, uint64_t inx)
{
    assert(inx < MAX_INTR_NO);
    void *addr = (void *)(GICD_ITARGETSRn(irq->disp_base_addr) +
                          ((inx >> 2) << 2));
    uint32_t bit_inx = inx % 4;
    uint32_t tmp = read_reg32((uint32_t *)addr);

    return (tmp >> (bit_inx << 3)) & 0xff;
}
static inline void gic2_set_edge_mode(gic_t *irq, int inx, int mode)
{
    uint32_t tmp = read_reg32(GICD_ICFGRn(irq->disp_base_addr) +
                              ((inx >> 4) << 2));

    tmp &= ~(0x3ul << ((inx % 16) << 1));
    tmp |= (!!mode << 1) << ((inx % 16) << 1);
    write_reg32(GICD_ICFGRn(irq->disp_base_addr) +
                    ((inx >> 4) << 2),
                tmp);
}
static inline int gic2_get_edge_mode(gic_t *irq, int inx)
{
    uint32_t tmp = read_reg32(GICD_ICFGRn(irq->disp_base_addr) +
                              ((inx >> 4) << 2));

    return ((tmp >> ((inx % 16) << 1)) & 0x3UL) >> 1;
}
static inline void gic_enable(gic_t *irq)
{
    write_reg32(GICD_CTLR(irq->disp_base_addr), TRUE);
}
static inline void gic_disable(gic_t *irq)
{
    write_reg32(GICD_CTLR(irq->disp_base_addr), FALSE);
}
static inline void gic2_set_prio(gic_t *irq, int inx, uint64_t prio)
{
    uint32_t tmp = read_reg32(GICD_IPRIORITYRn(irq->disp_base_addr) +
                              ((inx >> 2) << 2));

    tmp &= ~(0xffUL << ((inx % 4) << 3));
    tmp |= (prio & 0xffUL) << ((inx % 4) << 3);
    write_reg32(GICD_IPRIORITYRn(irq->disp_base_addr) +
                    ((inx >> 2) << 2),
                tmp);
}
static inline uint8_t gic2_get_prio(gic_t *irq, int inx)
{
    uint32_t tmp = read_reg32(GICD_IPRIORITYRn(irq->disp_base_addr) +
                              ((inx >> 2) << 2));

    return ((tmp) >> ((inx % 4) << 3)) & 0xffUL;
}

// cpu inter
static inline void gic2_cpu_enable(gic_t *irq)
{
    write_reg32(GICC_CTLR(irq->inter_base_addr), TRUE);
}
static inline void gic2_cpu_disable(gic_t *irq)
{
    write_reg32(GICC_CTLR(irq->inter_base_addr), FALSE);
}

static inline void gic_dist_init(gic_t *irq)
{
    gic_disable(irq);

    if (arch_get_current_cpu_id() == 0)
    {
        for (int i = 32; i < irq->irqs_number; i++)
        {
            gic2_set_unmask(irq, i);
            gic2_set_edge_mode(irq, i, 0);
            gic2_clear_active(irq, i);
        }
    }
    for (int i = 0; i < 16; i++)
    {
        gic2_set_unmask(irq, i);
    }

    gic_enable(irq);
}
static inline void gic_inter_init(gic_t *irq)
{
    // if (arch_get_current_cpu_id() == 0)
    {
        for (int i = 0; i < 32; i++)
        {
            gic2_set_prio(irq, i, 0);
        }
    }
    write_reg32(GICC_PMR(irq->inter_base_addr), 0xf0UL);

    gic2_cpu_enable(irq);
}
static inline void gic2_eoi_irq(gic_t *irq, int inx)
{
    write_reg32(GICC_EOIR(irq->inter_base_addr), inx);
}
static inline void gic_init(gic_t *irq, addr_t disp_addr, addr_t inter_addr)
{
    irq->irqs_number = ((read_reg32(GICD_TYPER(disp_addr)) & 0x1ful) + 1) * 32;
    if (irq->irqs_number > MAX_INTR_NO)
    {
        irq->irqs_number = MAX_INTR_NO;
    }
    irq->disp_base_addr = disp_addr;
    irq->inter_base_addr = inter_addr;

    printk("irqs_number %d, disp_base 0x%lx inter_base 0x%lx.\n", irq->irqs_number, irq->disp_base_addr, irq->inter_base_addr);

    gic_dist_init(irq);
    gic_inter_init(irq);
}
gic_t *arm_gicv2_get_global(void);
static inline uint32_t gicv2_get_irqnr(gic_t *m_gic)
{
    uint32_t irqstat =
        read_reg32(GICC_IAR(m_gic->inter_base_addr));
    uint32_t irqnr = irqstat & 0x3ff;

    return irqnr;
}
