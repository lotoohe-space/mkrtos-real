#include "mk_sys.h"

#define UART01x_DR 0x00         /* Data read or written from the interface. */
#define UART01x_RSR 0x04        /* Receive status register (Read). */
#define UART01x_ECR 0x04        /* Error clear register (Write). */
#define UART010_LCRH 0x08       /* Line control register, high byte. */
#define ST_UART011_DMAWM 0x08   /* DMA watermark configure register. */
#define UART010_LCRM 0x0C       /* Line control register, middle byte. */
#define ST_UART011_TIMEOUT 0x0C /* Timeout period register. */
#define UART010_LCRL 0x10       /* Line control register, low byte. */
#define UART010_CR 0x14         /* Control register. */
#define UART01x_FR 0x18         /* Flag register (Read only). */
#define UART010_IIR 0x1C        /* Interrupt identification register (Read). */
#define UART010_ICR 0x1C        /* Interrupt clear register (Write). */
#define ST_UART011_LCRH_RX 0x1C /* Rx line control register. */
#define UART01x_ILPR 0x20       /* IrDA low power counter register. */
#define UART011_IBRD 0x24       /* Integer baud rate divisor register. */
#define UART011_FBRD 0x28       /* Fractional baud rate divisor register. */
#define UART011_LCRH 0x2c       /* Line control register. */
#define UART011_LCRH_WLEN_8 0x60
#define ST_UART011_LCRH_TX 0x2c /* Tx Line control register. */
#define UART011_CR 0x30         /* Control register. */
#define UART011_IFLS 0x34       /* Interrupt fifo level select. */
#define UART011_IMSC 0x38       /* Interrupt mask. */
#define UART011_RIS 0x3c        /* Raw interrupt status. */
#define UART011_MIS 0x40        /* Masked interrupt status. */
#define UART011_ICR 0x44        /* Interrupt clear register. */
#define UART011_DMACR 0x48      /* DMA control register. */
#define ST_UART011_XFCR 0x50    /* XON/XOFF control register. */
#define ST_UART011_XON1 0x54    /* XON1 register. */
#define ST_UART011_XON2 0x58    /* XON2 register. */
#define ST_UART011_XOFF1 0x5C   /* XON1 register. */
#define ST_UART011_XOFF2 0x60   /* XON2 register. */
#define ST_UART011_ITCR 0x80    /* Integration test control register. */
#define ST_UART011_ITIP 0x84    /* Integration test input register. */
#define ST_UART011_ABCR 0x100   /* Autobaud control register. */
#define ST_UART011_ABIMSC 0x15C /* Autobaud interrupt mask/clear register. */

#define UART011_FR_RI 0x100
#define UART011_FR_TXFE 0x080
#define UART011_FR_RXFF 0x040
#define UART01x_FR_TXFF 0x020
#define UART01x_FR_RXFE 0x010
#define UART01x_FR_BUSY 0x008
#define UART01x_FR_DCD 0x004
#define UART01x_FR_DSR 0x002
#define UART01x_FR_CTS 0x001
#define UART01x_FR_TMSK (UART01x_FR_TXFF + UART01x_FR_BUSY)

#define UART011_CR_CTSEN 0x8000 /* CTS hardware flow control */
#define UART011_CR_RTSEN 0x4000 /* RTS hardware flow control */
#define UART011_CR_OUT2 0x2000  /* OUT2 */
#define UART011_CR_OUT1 0x1000  /* OUT1 */
#define UART011_CR_RTS 0x0800   /* RTS */
#define UART011_CR_DTR 0x0400   /* DTR */
#define UART011_CR_RXE 0x0200   /* receive enable */
#define UART011_CR_TXE 0x0100   /* transmit enable */
#define UART011_CR_LBE 0x0080   /* loopback enable */
#define UART010_CR_RTIE 0x0040
#define UART010_CR_TIE 0x0020
#define UART010_CR_RIE 0x0010
#define UART010_CR_MSIE 0x0008
#define ST_UART011_CR_OVSFACT 0x0008 /* Oversampling factor */
#define UART01x_CR_IIRLP 0x0004      /* SIR low power mode */
#define UART01x_CR_SIREN 0x0002      /* SIR enable */
#define UART01x_CR_UARTEN 0x0001     /* UART enable */

#define UART011_OEIM (1 << 10)  /* overrun error interrupt mask */
#define UART011_BEIM (1 << 9)   /* break error interrupt mask */
#define UART011_PEIM (1 << 8)   /* parity error interrupt mask */
#define UART011_FEIM (1 << 7)   /* framing error interrupt mask */
#define UART011_RTIM (1 << 6)   /* receive timeout interrupt mask */
#define UART011_TXIM (1 << 5)   /* transmit interrupt mask */
#define UART011_RXIM (1 << 4)   /* receive interrupt mask */
#define UART011_DSRMIM (1 << 3) /* DSR interrupt mask */
#define UART011_DCDMIM (1 << 2) /* DCD interrupt mask */
#define UART011_CTSMIM (1 << 1) /* CTS interrupt mask */
#define UART011_RIMIM (1 << 0)  /* RI interrupt mask */

#define UART011_OEIS (1 << 10)  /* overrun error interrupt status */
#define UART011_BEIS (1 << 9)   /* break error interrupt status */
#define UART011_PEIS (1 << 8)   /* parity error interrupt status */
#define UART011_FEIS (1 << 7)   /* framing error interrupt status */
#define UART011_RTIS (1 << 6)   /* receive timeout interrupt status */
#define UART011_TXIS (1 << 5)   /* transmit interrupt status */
#define UART011_RXIS (1 << 4)   /* receive interrupt status */
#define UART011_DSRMIS (1 << 3) /* DSR interrupt status */
#define UART011_DCDMIS (1 << 2) /* DCD interrupt status */
#define UART011_CTSMIS (1 << 1) /* CTS interrupt status */
#define UART011_RIMIS (1 << 0)  /* RI interrupt status */

#define UART011_OEIC (1 << 10)  /* overrun error interrupt clear */
#define UART011_BEIC (1 << 9)   /* break error interrupt clear */
#define UART011_PEIC (1 << 8)   /* parity error interrupt clear */
#define UART011_FEIC (1 << 7)   /* framing error interrupt clear */
#define UART011_RTIC (1 << 6)   /* receive timeout interrupt clear */
#define UART011_TXIC (1 << 5)   /* transmit interrupt clear */
#define UART011_RXIC (1 << 4)   /* receive interrupt clear */
#define UART011_DSRMIC (1 << 3) /* DSR interrupt clear */
#define UART011_DCDMIC (1 << 2) /* DCD interrupt clear */
#define UART011_CTSMIC (1 << 1) /* CTS interrupt clear */
#define UART011_RIMIC (1 << 0)  /* RI interrupt clear */

#define UART011_BASE_ADDR 0x9000000 //!< 寄存器基地址
#define UART011_FREQ 0x0            //!< 频率
#define UART011_DEFAULT_BAUD 115200
void uart_set_baud(int baud)
{
    uint32_t fi_val = UART011_FREQ * 4 / baud;

    write_reg32(UART011_BASE_ADDR + UART011_FBRD, fi_val & 0x3f);
    write_reg32(UART011_BASE_ADDR + UART011_IBRD, fi_val >> 6);
}
void uart_write_byte(uint8_t byte)
{
    int i = 3000000;

    while (i--)
    {
        if (!(read_reg32(UART011_BASE_ADDR + UART01x_FR) & UART01x_FR_TXFF))
        {
            break;
        }
    }
    write_reg32(UART011_BASE_ADDR + UART01x_DR, byte);
}
void uart_write_bytes(uint8_t *bytes, int len)
{
    for (int i = 0; i < len; i++)
    {
        uart_write_byte((bytes[i]));
    }
}
void uart_write_str(const char *str)
{
    for (int i = 0; str[i]; i++)
    {
        uart_write_byte((str[i]));
    }
}
void uart_init(void)
{
    write_reg32(UART011_BASE_ADDR + UART011_CR, UART01x_CR_UARTEN | UART011_CR_TXE | UART011_CR_RXE);
    if (UART011_FREQ)
    {
        uart_set_baud(UART011_DEFAULT_BAUD);
    }
    write_reg32(UART011_BASE_ADDR + UART011_LCRH, UART011_LCRH_WLEN_8);
    write_reg32(UART011_BASE_ADDR + UART011_IMSC, 0);

    int i = 4000000;

    while (i--)
    {
        if (!(read_reg32(UART011_BASE_ADDR + UART01x_FR) & UART01x_FR_BUSY))
        {
            break;
        }
    }
}
