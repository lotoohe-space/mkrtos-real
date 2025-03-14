#pragma once

#include <u_types.h>
#include <mk_drv.h>

/**
 * @brief pca9555 io definition
 */
#define PCA_IO0_PINS_0 0x0001  /*!< io 0 pins number 0 */
#define PCA_IO0_PINS_1 0x0002  /*!< io 0 pins number 1 */
#define PCA_IO0_PINS_2 0x0004  /*!< io 0 pins number 2 */
#define PCA_IO0_PINS_3 0x0008  /*!< io 0 pins number 3 */
#define PCA_IO0_PINS_4 0x0010  /*!< io 0 pins number 4 */
#define PCA_IO0_PINS_5 0x0020  /*!< io 0 pins number 5 */
#define PCA_IO0_PINS_6 0x0040  /*!< io 0 pins number 6 */
#define PCA_IO0_PINS_7 0x0080  /*!< io 0 pins number 7 */
#define PCA_IO1_PINS_0 0x0100  /*!< io 1 pins number 0 */
#define PCA_IO1_PINS_1 0x0200  /*!< io 1 pins number 1 */
#define PCA_IO1_PINS_2 0x0400  /*!< io 1 pins number 2 */
#define PCA_IO1_PINS_3 0x0800  /*!< io 1 pins number 3 */
#define PCA_IO1_PINS_4 0x1000  /*!< io 1 pins number 4 */
#define PCA_IO1_PINS_5 0x2000  /*!< io 1 pins number 5 */
#define PCA_IO1_PINS_6 0x4000  /*!< io 1 pins number 6 */
#define PCA_IO1_PINS_7 0x8000  /*!< io 1 pins number 7 */
#define PCA_IO_PINS_ALL 0xFFFF /*!< io all pins */

/**
 * @brief pca9555 i2c reg address
 */
#define PCA_REG_INPUT 0x00
#define PCA_REG_OUTPUT 0x02
#define PCA_REG_POLARITY 0x04
#define PCA_REG_CTRL 0x06

/**
 * @brief pca9555设备数据
 *
 */
typedef struct mk_pca9555
{
    void *priv_data;
} mk_pca9555_t;

enum pca9555_io_mode
{
    PCA9555_SET_INPUT_MODE,
    PCA9555_SET_OUTPUT_MODE,
    PCA9555_IO_RESET,
    PCA9555_IO_SET,
};
/**
 * @brief pca9555设备操作对象
 *
 */
typedef struct mk_pca9555_ops
{
    int (*pca9555_configure)(mk_pca9555_t *drv, enum pca9555_io_mode imode, uint16_t val);
    int (*pca9555_read)(mk_pca9555_t *drv, uint8_t *data, int len);
    int (*pca9555_write)(mk_pca9555_t *drv, uint8_t *dat, int len);
} mk_pca9555_ops_t;

/**
 * @brief pca9555设备
 *
 */
typedef struct mk_pca9555_dev
{
    mk_pca9555_t pca9555;
    mk_pca9555_ops_t *ops;
} mk_pca9555_dev_t;

int mk_pca9555_register(mk_dev_t *dev, mk_pca9555_dev_t *pin_drv);
