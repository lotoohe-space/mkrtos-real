/**
 * Copyright (c) 2011 Anup Patel.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * @file pl011.c
 * @author Anup Patel (anup@brainfault.org)
 * @brief PrimeCell PL011 serial emulator.
 * @details This source file implements the PrimeCell PL011 serial emulator.
 *
 * The source has been largely adapted from QEMU 0.14.xx hw/pl011.c
 *
 * Arm PrimeCell PL011 UART
 *
 * Copyright (c) 2006 CodeSourcery.
 * Written by Paul Brook
 *
 * The original code is licensed under the GPL.
 */

#include <guest_os.h>
#include <aarch64_emul.h>
#include <uvmm_dev_man.h>
#include <errno.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>

#define PL011_INT_TX 0x20
#define PL011_INT_RX 0x10

#define PL011_FLAG_TXFE 0x80
#define PL011_FLAG_RXFF 0x40
#define PL011_FLAG_TXFF 0x20
#define PL011_FLAG_RXFE 0x10

struct pl011_state
{
    guest_os_t *guest;
    // struct vmm_vserial *vser;
    // vmm_spinlock_t lock;/
    uint8_t id[8];
    uint32_t irq;
    uint32_t fifo_sz;
    uint32_t flags;
    uint32_t lcr;
    uint32_t cr;
    uint32_t dmacr;
    uint32_t int_enabled;
    uint32_t int_level;
    uint32_t ilpr;
    uint32_t ibrd;
    uint32_t fbrd;
    uint32_t ifl;
    int rd_trig;
    // struct fifo *rd_fifo;
};

static void pl011_set_irq(struct pl011_state *s, uint32_t level, uint32_t enabled)
{
    // if (level & enabled)
    // {
    //     vmm_devemu_emulate_irq(s->guest, s->irq, 1);
    // }
    // else
    // {
    //     vmm_devemu_emulate_irq(s->guest, s->irq, 0);
    // }
}

static void pl011_set_read_trigger(struct pl011_state *s)
{
#if 0
    /* The docs say the RX interrupt is triggered when the FIFO exceeds
       the threshold.  However linux only reads the FIFO in response to an
       interrupt.  Triggering the interrupt when the FIFO is non-empty seems
       to make things work.  */
    if (s->lcr & 0x10)
        s->read_trigger = (s->ifl >> 1) & 0x1c;
    else
#endif
    s->rd_trig = 1;
}

static int pl011_reg_read(struct pl011_state *s, uint32_t offset, uint32_t *dst)
{
    int rc = 0;
    uint8_t val = 0x0;
    bool_t set_irq = FALSE;
    uint32_t read_count = 0x0, level, enabled;

    // vmm_spin_lock(&s->lock);

    switch (offset >> 2)
    {
    case 0: /* UARTDR */
        s->flags &= ~PL011_FLAG_RXFF;
        // fifo_dequeue(s->rd_fifo, &val);TODO:这里读串口数据
        *dst = val;
        // read_count = fifo_avail(s->rd_fifo);
        if (read_count == 0)
        {
            s->flags |= PL011_FLAG_RXFE;
        }
        if (read_count == (s->rd_trig - 1))
        {
            s->int_level &= ~PL011_INT_RX;
        }
        set_irq = TRUE;
        level = s->int_level;
        enabled = s->int_enabled;
        break;
    case 1: /* UARTCR */
        *dst = 0;
        break;
    case 6: /* UARTFR */
        *dst = s->flags;
        break;
    case 8: /* UARTILPR */
        *dst = s->ilpr;
        break;
    case 9: /* UARTIBRD */
        *dst = s->ibrd;
        break;
    case 10: /* UARTFBRD */
        *dst = s->fbrd;
        break;
    case 11: /* UARTLCR_H */
        *dst = s->lcr;
        break;
    case 12: /* UARTCR */
        *dst = s->cr;
        break;
    case 13: /* UARTIFLS */
        *dst = s->ifl;
        break;
    case 14: /* UARTIMSC */
        *dst = s->int_enabled;
        break;
    case 15: /* UARTRIS */
        *dst = s->int_level;
        break;
    case 16: /* UARTMIS */
        *dst = s->int_level & s->int_enabled;
        break;
    case 18: /* UARTDMACR */
        *dst = s->dmacr;
        break;
    default:
        if (offset >= 0xfe0 && offset < 0x1000)
        {
            *dst = s->id[(offset - 0xfe0) >> 2];
        }
        else
        {
            rc = -EFAULT;
        }
        break;
    };

    // vmm_spin_unlock(&s->lock);

    if (set_irq)
    {
        pl011_set_irq(s, level, enabled);
    }

    return rc;
}

static int pl011_reg_write(struct pl011_state *s, uint32_t offset,
                           uint32_t src_mask, uint32_t src)
{
    uint8_t val;
    int rc = 0;
    bool_t set_irq = FALSE;
    bool_t recv_char = FALSE;
    uint32_t level = 0, enabled = 0;

    // vmm_spin_lock(&s->lock);

    switch (offset >> 2)
    {
    case 0: /* UARTDR */
        /* ??? Check if transmitter is enabled.  */
        val = src;
        recv_char = TRUE;
        s->int_level |= PL011_INT_TX;
        set_irq = TRUE;
        level = s->int_level;
        enabled = s->int_enabled;
        break;
    case 1: /* UARTCR */
        s->cr = (s->cr & src_mask) | (src & ~src_mask);
        break;
    case 6: /* UARTFR */
        /* Writes to Flag register are ignored.  */
        break;
    case 8: /* UARTUARTILPR */
        s->ilpr = (s->ilpr & src_mask) | (src & ~src_mask);
        break;
    case 9: /* UARTIBRD */
        s->ibrd = (s->ibrd & src_mask) | (src & ~src_mask);
        break;
    case 10: /* UARTFBRD */
        s->fbrd = (s->fbrd & src_mask) | (src & ~src_mask);
        break;
    case 11: /* UARTLCR_H */
        s->lcr = src;
        pl011_set_read_trigger(s);
        break;
    case 12: /* UARTCR */
        /* ??? Need to implement the enable and loopback bits.  */
        s->cr = (s->cr & src_mask) | (src & ~src_mask);
        break;
    case 13: /* UARTIFS */
        s->ifl = (s->ifl & src_mask) | (src & ~src_mask);
        pl011_set_read_trigger(s);
        break;
    case 14: /* UARTIMSC */
        s->int_enabled = (s->int_enabled & src_mask) |
                         (src & ~src_mask);
        set_irq = TRUE;
        level = s->int_level;
        enabled = s->int_enabled;
        break;
    case 17: /* UARTICR */
        s->int_level &= ~(src & ~src_mask);
        set_irq = TRUE;
        level = s->int_level;
        enabled = s->int_enabled;
        break;
    case 18: /* UARTDMACR */
        /* ??? DMA not implemented */
        s->dmacr = (s->dmacr & src_mask) | (src & ~src_mask);
        s->dmacr &= ~0x3;
        break;
    default:
        rc = -EFAULT;
        break;
    };

    // vmm_spin_unlock(&s->lock);

    if (recv_char)
    {
        // TODO:在这里写串口数据
        //  vmm_vserial_receive(s->vser, &val, 1);
        putc(val, stdout);
        fflush(stdout);
    }

    if (set_irq)
    {
        pl011_set_irq(s, level, enabled);
    }

    return rc;
}

// static bool_t pl011_vserial_can_send(struct vmm_vserial *vser)
// {
//     struct pl011_state *s = vmm_vserial_priv(vser);
// #if 0
// 	uint32_t rd_count;

// 	rd_count = fifo_avail(s->rd_fifo);
// 	if (s->lcr & 0x10) {
// 		return (rd_count < s->fifo_sz);
// 	} else {
// 		return (rd_count < 1);
// 	}
// #endif
//     return !fifo_isfull(s->rd_fifo);
// }

// static int pl011_vserial_send(struct vmm_vserial *vser, uint8_t data)
// {
//     bool_t set_irq = FALSE;
//     uint32_t rd_count, level, enabled;
//     struct pl011_state *s = vmm_vserial_priv(vser);

//     fifo_enqueue(s->rd_fifo, &data, TRUE);
//     rd_count = fifo_avail(s->rd_fifo);

//     vmm_spin_lock(&s->lock);
//     s->flags &= ~PL011_FLAG_RXFE;
//     if (s->cr & 0x10 || rd_count == s->fifo_sz)
//     {
//         s->flags |= PL011_FLAG_RXFF;
//     }
//     if (rd_count >= s->rd_trig)
//     {
//         s->int_level |= PL011_INT_RX;
//         set_irq = TRUE;
//         level = s->int_level;
//         enabled = s->int_enabled;
//     }
//     vmm_spin_unlock(&s->lock);

//     if (set_irq)
//     {
//         pl011_set_irq(s, level, enabled);
//     }

//     return 0;
// }

static int pl011_emulator_read8(vmm_dev_t *edev,
                                paddr_t offset,
                                uint8_t *dst)
{
    int rc;
    uint32_t regval = 0x0;

    rc = pl011_reg_read(edev->priv, offset, &regval);
    if (!rc)
    {
        *dst = regval & 0xFF;
    }

    return rc;
}

static int pl011_emulator_read16(vmm_dev_t *edev,
                                 paddr_t offset,
                                 uint16_t *dst)
{
    int rc;
    uint32_t regval = 0x0;

    rc = pl011_reg_read(edev->priv, offset, &regval);
    if (!rc)
    {
        *dst = regval & 0xFFFF;
    }

    return rc;
}

static int pl011_emulator_read32(vmm_dev_t *edev,
                                 paddr_t offset,
                                 uint32_t *dst)
{
    return pl011_reg_read(edev->priv, offset, dst);
}

static int pl011_emulator_write8(vmm_dev_t *edev,
                                 paddr_t offset,
                                 uint8_t src)
{
    return pl011_reg_write(edev->priv, offset, 0xFFFFFF00, src);
}

static int pl011_emulator_write16(vmm_dev_t *edev,
                                  paddr_t offset,
                                  uint16_t src)
{
    return pl011_reg_write(edev->priv, offset, 0xFFFF0000, src);
}

static int pl011_emulator_write32(vmm_dev_t *edev,
                                  paddr_t offset,
                                  uint32_t src)
{
    return pl011_reg_write(edev->priv, offset, 0x00000000, src);
}

static int pl011_emulator_reset(vmm_dev_t *edev)
{
    struct pl011_state *s = edev->priv;

    // vmm_spin_lock(&s->lock);

    s->rd_trig = 1;
    s->ifl = 0x12;
    s->cr = 0x300;
    s->flags = 0x90;

    // vmm_spin_unlock(&s->lock);

    return 0;
}
static int pl011_emulator_probe(guest_os_t *guest,
                                vmm_dev_t *edev,
                                const vmm_devtree_id_t *eid)
{
    int rc = 0;
    char name[64];
    struct pl011_state *s;

    s = calloc(1, sizeof(struct pl011_state));
    if (!s)
    {
        rc = -EFAULT;
        goto pl011_emulator_probe_done;
    }

    s->guest = guest;
    // INIT_SPIN_LOCK(&s->lock);

    if (eid->data)
    {
        s->id[0] = ((uint32_t *)eid->data)[0];
        s->id[1] = ((uint32_t *)eid->data)[1];
        s->id[2] = ((uint32_t *)eid->data)[2];
        s->id[3] = ((uint32_t *)eid->data)[3];
        s->id[4] = ((uint32_t *)eid->data)[4];
        s->id[5] = ((uint32_t *)eid->data)[5];
        s->id[6] = ((uint32_t *)eid->data)[6];
        s->id[7] = ((uint32_t *)eid->data)[7];
    }

    // rc = vmm_devtree_read_uint32_t_atindex(edev->node,
    //                                        VMM_DEVTREE_INTERRUPTS_ATTR_NAME,
    //                                        &s->irq, 0);
    // if (rc)
    // {
    //     goto pl011_emulator_probe_freestate_fail;
    // }

    // rc = vmm_devtree_read_uint32_t(edev->node, "fifo_size", &s->fifo_sz);
    // if (rc)
    // {
    //     goto pl011_emulator_probe_freestate_fail;
    // }

    // s->rd_fifo = fifo_alloc(1, s->fifo_sz);
    // if (!s->rd_fifo)
    // {
    //     rc = -EFAULT;
    //     goto pl011_emulator_probe_freestate_fail;
    // }

    // strlcpy(name, guest->name, sizeof(name));
    // strlcat(name, "/", sizeof(name));
    // if (strlcat(name, edev->node->name, sizeof(name)) >= sizeof(name))
    // {
    //     rc = -ENAMETOOLONG;
    //     goto pl011_emulator_probe_freerbuf_fail;
    // }
    // s->vser = vmm_vserial_create(name,
    //                              &pl011_vserial_can_send,
    //                              &pl011_vserial_send,
    //                              s->fifo_sz, s);
    // if (!(s->vser))
    // {
    //     goto pl011_emulator_probe_freerbuf_fail;
    // }
    strcpy(guest->name, "pl011");

    edev->priv = s;

    goto pl011_emulator_probe_done;

// pl011_emulator_probe_freerbuf_fail:
//     fifo_free(s->rd_fifo);
pl011_emulator_probe_freestate_fail:
    free(s);
pl011_emulator_probe_done:
    return rc;
}

static int pl011_emulator_remove(vmm_dev_t *edev)
{
    struct pl011_state *s = edev->priv;

    if (s)
    {
        // vmm_vserial_destroy(s->vser);
        // fifo_free(s->rd_fifo);
        free(s);
        edev->priv = NULL;
    }

    return 0;
}

static uint32_t pl011_configs[] = {
    /* === arm === */
    /* id0 */ 0x11,
    /* id1 */ 0x10,
    /* id2 */ 0x14,
    /* id3 */ 0x00,
    /* id4 */ 0x0d,
    /* id5 */ 0xf0,
    /* id6 */ 0x05,
    /* id7 */ 0xb1,
    /* === luminary === */
    /* id0 */ 0x11,
    /* id1 */ 0x00,
    /* id2 */ 0x18,
    /* id3 */ 0x01,
    /* id4 */ 0x0d,
    /* id5 */ 0xf0,
    /* id6 */ 0x05,
    /* id7 */ 0xb1,
};

static vmm_devtree_id_t pl011_emuid_table[] = {
    {
        .type = "serial",
        .compatible = "primecell,arm,pl011",
        .data = &pl011_configs[0],
    },
    {
        .type = "serial",
        .compatible = "primecell,luminary,pl011",
        .data = &pl011_configs[8],
    },
    {/* end of list */},
};

static vmm_simul_t pl011_emulator = {
    .name = "pl011",
    .match_tab = pl011_emuid_table,
    // .endian = VMM_DEVEMU_LITTLE_ENDIAN,
    .probe = pl011_emulator_probe,
    .read_u8 = pl011_emulator_read8,
    .write_u8 = pl011_emulator_write8,
    .read_u16 = pl011_emulator_read16,
    .write_u16 = pl011_emulator_write16,
    .read_u32 = pl011_emulator_read32,
    .write_u32 = pl011_emulator_write32,
    .reset = pl011_emulator_reset,
    .remove = pl011_emulator_remove,
};

static int pl011_emulator_init(void)
{
    return uvmm_register_emulator_dev(&pl011_emulator);
}

static void pl011_emulator_exit(void)
{
    uvmm_unregister_emulator_dev(&pl011_emulator);
}

UVMM_DECLARE_MODULE("pl011 device simul",
                    "zhangzheng",
                    2,
                    pl011_emulator_init,
                    pl011_emulator_exit);
                    
