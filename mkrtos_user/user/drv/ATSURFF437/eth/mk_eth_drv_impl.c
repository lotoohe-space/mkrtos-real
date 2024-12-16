

#include <u_types.h>
#include <errno.h>
#include <libfdt.h>
#include <mk_dev.h>
#include <stdio.h>
#include <assert.h>
#include "mk_drv.h"
#include "mk_eth_drv.h"
#include "mk_dtb_parse.h"
#include <u_prot.h>
#include <u_vmam.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "at_surf_f437_board_emac.h"
#include "mk_eth_drv_impl.h"
typedef struct eth_priv_data
{
} eth_priv_data_t;

static eth_priv_data_t eth_priv_data;

static int eth_bus_configure(mk_eth_t *drv, enum mk_eth_ioctl_op op, mk_ioctl_set_info_t *info)
{
    assert(drv);
    eth_priv_data_t *priv_data = drv->priv_data;
    switch (op)
    {
    case ETH_SET_MAC_ADDR:
        emac_local_address_set(info->mac_addr);
        break;
    default:
        break;
    }

    return 0;
}
static int eth_bus_read(mk_eth_t *drv, uint8_t *data, int len)
{
    int ret = len;

    ret = emac_read_packet((uint8_t *)data, len);

    return ret;
}
static int eth_bus_write(mk_eth_t *drv, uint8_t *data, int len)
{
    int ret;

    ret = emac_send_packet((uint8_t *)data, len);
    return ret;
}

static mk_eth_ops_t eth_ops = {
    .eth_configure = eth_bus_configure,
    .eth_read = eth_bus_read,
    .eth_write = eth_bus_write,
};
static mk_eth_dev_t eth_dev = {
    .ops = &eth_ops,
    .eth.priv_data = &eth_priv_data,
};

static int eth_probe(mk_dev_t *dev)
{
    int ret;

    /* ioremap */
    ret = dev_regs_map(dev, dev->dtb);
    if (ret < 0)
    {
        printf("eth dev regs map failed.\n");
        return -ENOMEM;
    }
    if (emac_system_init() != SUCCESS)
    {
        printf("emac init fail.\n");
        return -1;
    }
    printf("eth init success.\r\n");
    /* 注册eth设备 */
    // mk_eth_register(dev, &eth_dev);
    return 0;
}

static mk_drv_compatible_t drv_compatilbe[] =
    {
        {"at32f43x,eth"},
        {NULL},
};

static mk_drv_t eth_drv =
    {
        .compatilbe = drv_compatilbe,
        .probe = eth_probe,
        .data = NULL,
};
void drv_eth_init(void)
{
    mk_drv_register(&eth_drv);
}
