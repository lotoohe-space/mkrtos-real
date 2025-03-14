

#include <u_types.h>
#include <errno.h>
#include <libfdt.h>
#include <mk_dev.h>
#include <stdio.h>
#include <assert.h>
#include "mk_drv.h"
#include "mk_pca9555_drv.h"
#include "mk_dtb_parse.h"
#include <u_prot.h>
#include <u_vmam.h>
#include "at32f435_437_crm.h"
#include "mk_pin_drv.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "pca9555.h"
typedef struct pca9555_priv_data
{
    pca9555_t pca9555_data;
} pca9555_priv_data_t;

static pca9555_priv_data_t pca9555_priv_data;

static int pca9555_configure(mk_pca9555_t *drv, enum pca9555_io_mode imode, uint16_t val)
{
    assert(drv);
    pca9555_priv_data_t *priv_data = drv->priv_data;

    int ret = -ENOSYS;

    switch (imode)
    {
    case PCA9555_SET_INPUT_MODE:
    case PCA9555_SET_OUTPUT_MODE:
        ret = pca9555_set_mode(&priv_data->pca9555_data, imode, val);
        if (ret < 0)
        {
            printf("%s:%d ret:%d\n", __func__, __LINE__, ret);
        }
        break;
    case PCA9555_IO_RESET:
        ret = pca9555_output_write(&priv_data->pca9555_data, priv_data->pca9555_data.odata & (~val));
        break;
    case PCA9555_IO_SET:
        ret = pca9555_output_write(&priv_data->pca9555_data, priv_data->pca9555_data.odata | val);
        break;
    default:
        break;
    }

    return ret;
}
static int pca9555_read(mk_pca9555_t *drv, uint8_t *data, int len)
{
    int ret;
    pca9555_priv_data_t *priv_data = drv->priv_data;
    assert(drv);

    if (len < sizeof(priv_data->pca9555_data.idata))
    {
        return -EINVAL;
    }

    ret = pca9555_input_read(&priv_data->pca9555_data);
    if (ret < 0)
    {
        return ret;
    }
    memcpy(data, &priv_data->pca9555_data.idata, sizeof(priv_data->pca9555_data.idata));

    return sizeof(priv_data->pca9555_data.idata);
}
static int pca9555_write(mk_pca9555_t *drv, uint8_t *data, int len)
{
    int ret;
    pca9555_priv_data_t *priv_data = drv->priv_data;

    assert(drv);

    if (len < 2)
    {
        return -EINVAL;
    }
    ret = pca9555_output_write(&priv_data->pca9555_data, *((uint16_t *)data));
    if (ret < 0)
    {
        printf("pca9555 write error, ret: %d.\n", ret);
        return ret;
    }

    return len;
}

static mk_pca9555_ops_t pca9555_ops = {
    .pca9555_configure = pca9555_configure,
    .pca9555_read = pca9555_read,
    .pca9555_write = pca9555_write,
};
static mk_pca9555_dev_t pca9555_dev = {
    .ops = &pca9555_ops,
    .pca9555.priv_data = &pca9555_priv_data,
};
static int pca9555_dtb_parse(mk_dev_t *dev, pca9555_t *data)
{
    const umword_t *val_raw;
    int len;
    int ret;

    val_raw = fdt_getprop(dev->dtb, dev->dtb_offset,
                          "i2c_addr", &len);
    if (!val_raw)
    {
        return -ENODEV;
    }
    if (len < sizeof(void *) * 1)
    {
        return -ENODEV;
    }
    data->i2c_addr = fdt32_to_cpu((fdt32_t)val_raw[0]);
    data->i2c_name = fdt_getprop(dev->dtb, dev->dtb_offset, "i2c", &len);
    if (!data->i2c_name)
    {
        printf("i2c dev not set.");
        return -ENODEV;
    }
    printf("i2c name:%s.\n", data->i2c_name);
    return 0;
}
static int pca9555_probe(mk_dev_t *dev)
{
    int ret;

    ret = pca9555_dtb_parse(dev, &pca9555_priv_data.pca9555_data);
    if (ret < 0)
    {
        printf("pca9555 dtb parse failed.\n");
        return ret;
    }
    /* ioremap */
    ret = dev_regs_map(dev, dev->dtb);
    if (ret < 0)
    {
        printf("pca9555 dev regs map failed.\n");
        return ret;
    }

    ret = pca9555_init(&pca9555_priv_data.pca9555_data);
    if (ret < 0)
    {
        printf("pca9555 dev init failed.\n");
        return ret;
    }
    /* 注册pca9555设备 */
    mk_pca9555_register(dev, &pca9555_dev);
    return 0;
}

static mk_drv_compatible_t drv_compatilbe[] =
    {
        {"at32f43x,pca9555"},
        {NULL},
};

static mk_drv_t pca9555_drv =
    {
        .compatilbe = drv_compatilbe,
        .probe = pca9555_probe,
        .data = NULL,
};
void drv_pca9555_init(void)
{
    mk_drv_register(&pca9555_drv);
}
