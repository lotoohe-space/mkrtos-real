#pragma once

#include <u_types.h>
#include <mk_drv.h>
#include <sys/types.h>
#define RT_I2C_WR              0x0000        /* 写标志，不可以和读标志进行“|”操作 */
#define RT_I2C_RD              (1u << 0)     /* 读标志，不可以和写标志进行“|”操作 */
#define RT_I2C_ADDR_10BIT      (1u << 2)     /* 10 位地址模式 */
#define RT_I2C_NO_START        (1u << 4)     /* 无开始条件 */
#define RT_I2C_IGNORE_NACK     (1u << 5)     /* 忽视 NACK */
#define RT_I2C_NO_READ_ACK     (1u << 6)     /* 读的时候不发送 ACK */
#define RT_I2C_NO_STOP         (1u << 7)     /* 不发送结束位 */

/**
 * @brief i2c设备数据
 *
 */
typedef struct mk_i2c
{
    umword_t freq;     /*总线频率*/
    umword_t i2c_addr; /*i2c addr*/
    umword_t mem_addr;
    void *priv_data;
} mk_i2c_t;
/**
 * @brief i2c设备操作对象
 *
 */
typedef struct mk_i2c_ops
{
    int (*i2c_bus_configure)(mk_i2c_t *drv, umword_t freq);
    int (*i2c_bus_read)(mk_i2c_t *drv, uint8_t *data, int len, uint16_t flags, uint16_t addr);
    int (*i2c_bus_write)(mk_i2c_t *drv, uint8_t *dat, int len, uint16_t flags, uint16_t addr);
} mk_i2c_ops_t;

/**
 * @brief i2c设备
 *
 */
typedef struct mk_i2c_dev
{
    mk_i2c_t i2c;
    mk_i2c_ops_t *ops;
} mk_i2c_dev_t;

enum mk_i2c_op
{
    MK_I2C_SET_FREQ, /*设置波特率*/
};

int mk_i2c_register(mk_dev_t *dev, mk_i2c_dev_t *pin_drv);
