#pragma once

#include <u_types.h>
#include <mk_drv.h>
#include <sys/types.h>
enum mk_eth_ioctl_op
{
    ETH_SET_MAC_ADDR,
};

typedef struct mk_ioctl_set_info
{
    uint8_t mac_addr[6];
} mk_ioctl_set_info_t;
/**
 * @brief eth设备数据
 *
 */
typedef struct mk_eth
{
    void *priv_data;
} mk_eth_t;
/**
 * @brief eth设备操作对象
 *
 */
typedef struct mk_eth_ops
{
    int (*eth_configure)(mk_eth_t *drv, enum mk_eth_ioctl_op op, mk_ioctl_set_info_t *info);
    int (*eth_read)(mk_eth_t *drv, uint8_t *data, int len);
    int (*eth_write)(mk_eth_t *drv, uint8_t *dat, int len);
} mk_eth_ops_t;

/**
 * @brief eth设备
 *
 */
typedef struct mk_eth_dev
{
    mk_eth_t eth;
    mk_eth_ops_t *ops;
} mk_eth_dev_t;

int mk_eth_register(mk_dev_t *dev, mk_eth_dev_t *pin_drv);
