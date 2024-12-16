

#include <u_types.h>
#include <errno.h>
#include <libfdt.h>
#include <mk_dev.h>
#include <stdio.h>
#include <assert.h>
#include "mk_drv.h"
#include "mk_i2c_drv.h"
#include "mk_dtb_parse.h"
#include <u_prot.h>
#include <u_vmam.h>
#include "at32f435_437_crm.h"
#include "i2c_application.h"
#include "mk_pin_drv.h"
#include "u_sleep.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
typedef struct i2c_priv_data
{
    i2c_handle_type hi2cx;
} i2c_priv_data_t;

static i2c_priv_data_t i2c_priv_data;

/**
 * @brief pca9555 pin definition
 */
#define PCA_I2C_ADDRESS 0x40

#define I2C_DEFAULT_TIMEOUT 0xffffff

#define PCA_I2C_CLKCTRL_100K 0x70F03C6B // 100K
#define PCA_I2C_CLKCTRL_200K 0x30F03C6B // 200K
#define PCA_I2C_CLKCTRL_400K 0x10F03D6B // 400K

#define PCA_I2C I2C2
#define PCA_I2C_CLK CRM_I2C2_PERIPH_CLOCK

#define PCA_I2C_SCL_GPIO_CLK CRM_GPIOH_PERIPH_CLOCK
#define PCA_I2C_SCL_GPIO_PIN GPIO_PINS_2
#define PCA_I2C_SCL_GPIO_PINS_SOURCE GPIO_PINS_SOURCE2
#define PCA_I2C_SCL_GPIO_PORT GPIOH
#define PCA_I2C_SCL_GPIO_MUX GPIO_MUX_4

#define PCA_I2C_SDA_GPIO_CLK CRM_GPIOH_PERIPH_CLOCK
#define PCA_I2C_SDA_GPIO_PIN GPIO_PINS_3
#define PCA_I2C_SDA_GPIO_PINS_SOURCE GPIO_PINS_SOURCE3
#define PCA_I2C_SDA_GPIO_PORT GPIOH
#define PCA_I2C_SDA_GPIO_MUX GPIO_MUX_4

static int pin_fd;
#define DEV_PIN_PATH "/pin"

/**
 * @brief  initializes peripherals used by the i2c.
 * @param  none
 * @retval none
 */
int i2c_lowlevel_init(i2c_handle_type *hi2c)
{
    gpio_init_type gpio_init_structure;

    if (hi2c->i2cx == PCA_I2C)
    {
        int ret;
        /* i2c periph clock enable */
        crm_periph_clock_enable(PCA_I2C_CLK, TRUE);
        crm_periph_clock_enable(PCA_I2C_SCL_GPIO_CLK, TRUE);
        crm_periph_clock_enable(PCA_I2C_SDA_GPIO_CLK, TRUE);
again:
        pin_fd = open(DEV_PIN_PATH, O_RDWR, 0777);

        if (pin_fd < 0)
        {
            u_sleep_ms(50);
            goto again;
        }

        ret = ioctl(pin_fd, MK_PIN_SET_MODE, ((mk_pin_cfg_t){
                                                  .mode = MK_PIN_MODE_MUX_OUTPUT_OD,
                                                  .pin = (7 * 16) + 2,
                                                  .cfg = PCA_I2C_SCL_GPIO_MUX,
                                              })
                                                 .cfg_raw);
        if (ret < 0)
        {
            printf("gpio init failed.\n");
            return ret;
        }
        ret = ioctl(pin_fd, MK_PIN_SET_MODE, ((mk_pin_cfg_t){
                                                  .mode = MK_PIN_MODE_MUX_OUTPUT_OD,
                                                  .pin = (7 * 16) + 3,
                                                  .cfg = PCA_I2C_SDA_GPIO_MUX,
                                              })
                                                 .cfg_raw);
        if (ret < 0)
        {
            printf("gpio init failed.\n");
            return ret;
        }
#if 0
        /* configure and enable i2c interrupt */
        nvic_irq_enable(I2Cx_EVT_IRQn, 0, 0);
        nvic_irq_enable(I2Cx_ERR_IRQn, 0, 0);

        /* configure and enable i2c dma channel interrupt */
        nvic_irq_enable(I2Cx_DMA_TX_IRQn, 0, 0);
        nvic_irq_enable(I2Cx_DMA_RX_IRQn, 0, 0);

        /* i2c dma tx and rx channels configuration */
        /* enable the dma clock */
        crm_periph_clock_enable(I2Cx_DMA_CLK, TRUE);

        /* i2c dma channel configuration */
        hi2c->dma_tx_channel = I2Cx_DMA_TX_Channel;
        hi2c->dma_rx_channel = I2Cx_DMA_RX_Channel;

        dma_reset(hi2c->dma_tx_channel);
        dma_reset(hi2c->dma_rx_channel);

        hi2c->dma_init_struct.peripheral_base_addr = (uint32_t)&hi2c->i2cx->txdt;
        hi2c->dma_init_struct.memory_base_addr = 0;
        hi2c->dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
        hi2c->dma_init_struct.buffer_size = 0xFFFF;
        hi2c->dma_init_struct.peripheral_inc_enable = FALSE;
        hi2c->dma_init_struct.memory_inc_enable = TRUE;
        hi2c->dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
        hi2c->dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
        hi2c->dma_init_struct.loop_mode_enable = FALSE;
        hi2c->dma_init_struct.priority = DMA_PRIORITY_LOW;

        dma_init(hi2c->dma_tx_channel, &hi2c->dma_init_struct);
        dma_init(hi2c->dma_rx_channel, &hi2c->dma_init_struct);

        dmamux_init(I2Cx_DMA_TX_DMAMUX_Channel, I2Cx_DMA_TX_DMAREQ);
        dmamux_init(I2Cx_DMA_RX_DMAMUX_Channel, I2Cx_DMA_RX_DMAREQ);

        dmamux_enable(I2Cx_DMA, TRUE);
#endif
        /* config i2c */
        i2c_init(hi2c->i2cx, 0x0F, PCA_I2C_CLKCTRL_400K);
    }
}
static int i2c_bus_configure(mk_i2c_t *drv, umword_t freq)
{
    assert(drv);
    i2c_priv_data_t *priv_data = drv->priv_data;
    drv->freq = freq;
    i2c_init(priv_data->hi2cx.i2cx, 0x0F, drv->freq);
    return 0;
}
static int i2c_bus_read(mk_i2c_t *drv, uint8_t *data, int len, uint16_t flags, uint16_t addr)
{
    i2c_status_type i2c_status;
    i2c_priv_data_t *priv_data = drv->priv_data;
    assert(drv);
    int r_len = len;
    /*TODO:接收数据应该用dma或者中断接收，在中断中接收*/
    i2c_transmit_set(priv_data->hi2cx.i2cx, addr, len,
                     (flags & RT_I2C_NO_STOP) ? I2C_SOFT_STOP_MODE : I2C_AUTO_STOP_MODE,
                     (flags & RT_I2C_NO_START) ? I2C_WITHOUT_START : I2C_GEN_START_READ);
    // if (!(flags & RT_I2C_NO_START))
    // {
    //     /* wait for the tdis flag to be set */
    //     if (i2c_wait_flag(&priv_data->hi2cx, I2C_TDIS_FLAG, I2C_EVENT_CHECK_ACKFAIL, I2C_DEFAULT_TIMEOUT) != I2C_OK)
    //     {
    //         return -EIO;
    //     }
    // }
    while (r_len > 0)
    {
        /* wait for the rdbf flag to be set  */
        if (i2c_wait_flag(&priv_data->hi2cx, I2C_RDBF_FLAG, I2C_EVENT_CHECK_ACKFAIL, I2C_DEFAULT_TIMEOUT) != I2C_OK)
        {
            return -EIO;
        }

        /* read data */
        (*data++) = i2c_data_receive(priv_data->hi2cx.i2cx);
        r_len--;

        // if (len != 0)
        // {
        //     /* wait for the tcrld flag to be set  */
        //     if (i2c_wait_flag(&priv_data->hi2cx, I2C_TCRLD_FLAG, I2C_EVENT_CHECK_NONE, I2C_DEFAULT_TIMEOUT) != I2C_OK)
        //     {
        //         return -EIO;
        //     }

        //     /* continue transfer */
        //     i2c_transmit_set(priv_data->hi2cx.i2cx, addr, 1, I2C_AUTO_STOP_MODE, I2C_WITHOUT_START);
        // }
    }
    if (!(flags & RT_I2C_NO_STOP))
    {
        /* wait for the stop flag to be set  */
        if (i2c_wait_flag(&priv_data->hi2cx, I2C_STOPF_FLAG, I2C_EVENT_CHECK_ACKFAIL, I2C_DEFAULT_TIMEOUT) != I2C_OK)
        {
            return -EIO;
        }

        /* clear stop flag */
        i2c_flag_clear(priv_data->hi2cx.i2cx, I2C_STOPF_FLAG);

        /* reset ctrl2 register */
        priv_data->hi2cx.i2cx->ctrl2_bit.saddr = 0;
        priv_data->hi2cx.i2cx->ctrl2_bit.readh10 = 0;
        priv_data->hi2cx.i2cx->ctrl2_bit.cnt = 0;
        priv_data->hi2cx.i2cx->ctrl2_bit.rlden = 0;
        priv_data->hi2cx.i2cx->ctrl2_bit.dir = 0;
    }
    return len;
}
static int i2c_bus_write(mk_i2c_t *drv, uint8_t *data, int len, uint16_t flags, uint16_t addr)
{
    assert(drv);
    i2c_status_type i2c_status;
    i2c_priv_data_t *priv_data = drv->priv_data;

    int w_len = len;

    /* 等待不忙 */
    if (i2c_wait_flag(&priv_data->hi2cx, I2C_BUSYF_FLAG, I2C_EVENT_CHECK_NONE, I2C_DEFAULT_TIMEOUT) != I2C_OK)
    {
        return -EIO;
    }

    i2c_transmit_set(priv_data->hi2cx.i2cx, addr, len,
                     (flags & RT_I2C_NO_STOP) ? I2C_SOFT_STOP_MODE : I2C_AUTO_STOP_MODE,
                     (flags & RT_I2C_NO_START) ? I2C_WITHOUT_START : I2C_GEN_START_WRITE);
    /* wait for the tdis flag to be set */
    if (i2c_wait_flag(&priv_data->hi2cx, I2C_TDIS_FLAG, I2C_EVENT_CHECK_ACKFAIL, I2C_DEFAULT_TIMEOUT) != I2C_OK)
    {
        return -EIO;
    }

    while (w_len > 0)
    {
        /* wait for the tdis flag to be set */
        if (i2c_wait_flag(&priv_data->hi2cx, I2C_TDIS_FLAG, I2C_EVENT_CHECK_ACKFAIL, I2C_DEFAULT_TIMEOUT) != I2C_OK)
        {
            return -EIO;
        }
        /* send data */
        i2c_data_send(priv_data->hi2cx.i2cx, *data++);
        w_len--;

        // if (len == 0)
        // {
        //     /* wait for the tcrld flag to be set  */
        //     if (i2c_wait_flag(&priv_data->hi2cx, I2C_TCRLD_FLAG, I2C_EVENT_CHECK_ACKFAIL, I2C_DEFAULT_TIMEOUT) != I2C_OK)
        //     {
        //         return -EIO;
        //     }

        //     /* continue transfer */
        //     i2c_transmit_set(priv_data->hi2cx.i2cx, addr, 1, I2C_AUTO_STOP_MODE, I2C_WITHOUT_START);
        // }
    }
    if (!(flags & RT_I2C_NO_STOP))
    {
        /* wait for the stop flag to be set  */
        if (i2c_wait_flag(&priv_data->hi2cx, I2C_STOPF_FLAG, I2C_EVENT_CHECK_ACKFAIL, I2C_DEFAULT_TIMEOUT) != I2C_OK)
        {
            return -EIO;
        }
        /* clear stop flag */
        i2c_flag_clear(priv_data->hi2cx.i2cx, I2C_STOPF_FLAG);
        /* reset ctrl2 register */
        priv_data->hi2cx.i2cx->ctrl2_bit.saddr = 0;
        priv_data->hi2cx.i2cx->ctrl2_bit.readh10 = 0;
        priv_data->hi2cx.i2cx->ctrl2_bit.cnt = 0;
        priv_data->hi2cx.i2cx->ctrl2_bit.rlden = 0;
        priv_data->hi2cx.i2cx->ctrl2_bit.dir = 0;
    }
    else
    {
        /* wait for the tdc flag to be set */
        if (i2c_wait_flag(&priv_data->hi2cx, I2C_TDC_FLAG, I2C_EVENT_CHECK_NONE, I2C_DEFAULT_TIMEOUT) != I2C_OK)
        {
            return I2C_ERR_STEP_4;
        }
    }

    return len;
}

static mk_i2c_ops_t i2c_ops = {
    .i2c_bus_configure = i2c_bus_configure,
    .i2c_bus_read = i2c_bus_read,
    .i2c_bus_write = i2c_bus_write,
};
static mk_i2c_dev_t i2c_dev = {
    .ops = &i2c_ops,
    .i2c.priv_data = &i2c_priv_data,
};

static int i2c_probe(mk_dev_t *dev)
{
    int ret;

    /* ioremap */
    ret = dev_regs_map(dev, dev->dtb);
    if (ret < 0)
    {
        printf("i2c dev regs map failed.\n");
        return -ENOMEM;
    }
    i2c_priv_data.hi2cx.i2cx = PCA_I2C;

    i2c_config(&i2c_priv_data.hi2cx);
    /* 注册i2c设备 */
    mk_i2c_register(dev, &i2c_dev);
    return 0;
}

static mk_drv_compatible_t drv_compatilbe[] =
    {
        {"at32f43x,i2c2"},
        {NULL},
};

static mk_drv_t i2c_drv =
    {
        .compatilbe = drv_compatilbe,
        .probe = i2c_probe,
        .data = NULL,
};
void drv_i2c_init(void)
{
    mk_drv_register(&i2c_drv);
}
