/*
 * @copyright (c) 2023, MR Development Team
 *
 * @license SPDX-License-Identifier: Apache-2.0
 *
 * @date 2023-11-01    MacRsh       First version
 */

#ifndef _MR_SPI_H_
#define _MR_SPI_H_

#include "mr_api.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef MR_USING_SPI

/**
 * @brief SPI host/slave.
 */
#define MR_SPI_HOST                     (0)                         /**< SPI host */
#define MR_SPI_SLAVE                    (1)                         /**< SPI slave */

/**
 * @brief SPI mode.
 */
#define MR_SPI_MODE_0                   (0)                         /**< CPOL = 0, CPHA = 0 */
#define MR_SPI_MODE_1                   (1)                         /**< CPOL = 0, CPHA = 1 */
#define MR_SPI_MODE_2                   (2)                         /**< CPOL = 1, CPHA = 0 */
#define MR_SPI_MODE_3                   (3)                         /**< CPOL = 1, CPHA = 1 */

/**
 * @brief SPI data bits.
 */
#define MR_SPI_DATA_BITS_8              (8)                         /**< 8 bits data */
#define MR_SPI_DATA_BITS_16             (16)                        /**< 16 bits data */
#define MR_SPI_DATA_BITS_32             (32)                        /**< 32 bits data */

/**
 * @brief SPI bit order.
 */
#define MR_SPI_BIT_ORDER_MSB            (0)                         /**< MSB first */
#define MR_SPI_BIT_ORDER_LSB            (1)                         /**< LSB first */

/**
 * @brief SPI offset bits.
 */
#define MR_SPI_OFF_BITS_8               (8)                         /**< 8 bits offset */
#define MR_SPI_OFF_BITS_16              (16)                        /**< 16 bits offset */
#define MR_SPI_OFF_BITS_32              (32)                        /**< 32 bits offset */

/**
 * @brief SPI default configuration.
 */
#define MR_SPI_CONFIG_DEFAULT           \
{                                       \
    3000000,                            \
    MR_SPI_HOST,                        \
    MR_SPI_MODE_0,                      \
    MR_SPI_DATA_BITS_8,                 \
    MR_SPI_BIT_ORDER_MSB,               \
    MR_SPI_OFF_BITS_8,                  \
}

/**
 * @brief SPI device configuration structure.
 */
struct mr_spi_config
{
    uint32_t baud_rate;                                             /**< Baud rate */
    uint32_t host_slave: 1;                                         /**< Host/slave */
    uint32_t mode: 2;                                               /**< Mode */
    uint32_t data_bits: 6;                                          /**< Data bits */
    uint32_t bit_order: 1;                                          /**< Bit order */
    uint32_t off_bits: 6;                                           /**< Offset bits */
    uint32_t reserved: 16;
};

/**
 * @brief SPI control command.
 */
#define MR_IOCTL_SPI_TRANSFER           ((0x1|0x80) << 16)          /**< Transfer */

/**
 * @brief SPI transfer structure.
 */
struct mr_spi_transfer
{
    void *rd_buf;                                                   /**< Read buffer */
    const void *wr_buf;                                             /**< Write buffer */
    size_t size;                                                    /**< Transfer size */
};

/**
 * @brief SPI bus structure.
 */
struct mr_spi_bus
{
    struct mr_dev dev;                                              /**< Device */

    struct mr_spi_config config;                                    /**< Configuration */
    volatile uint32_t lock;                                         /**< Lock */
    void *owner;                                                    /**< Owner */
};

/**
 * @brief SPI bus operations structure.
 */
struct mr_spi_bus_ops
{
    int (*configure)(struct mr_spi_bus *spi_bus, struct mr_spi_config *config);
    void (*write)(struct mr_spi_bus *spi_bus, uint32_t data);
    uint32_t (*read)(struct mr_spi_bus *spi_bus);
    void (*cs_write)(struct mr_spi_bus *spi_bus, int cs_pin, int level);
    int (*cs_read)(struct mr_spi_bus *spi_bus, int cs_pin);
};

/**
 * @brief SPI CS active level.
 */
#define MR_SPI_CS_ACTIVE_LOW            (0)                         /**< Active low */
#define MR_SPI_CS_ACTIVE_HIGH           (1)                         /**< Active high */
#define MR_SPI_CS_ACTIVE_HARDWARE       (2)                         /**< Hardware */

/**
 * @struct SPI device structure.
 */
struct mr_spi_dev
{
    struct mr_dev dev;                                              /**< Device */

    struct mr_spi_config config;                                    /**< Config */
    struct mr_ringbuf rd_fifo;                                      /**< Read FIFO */
    size_t rd_bufsz;                                                /**< Read buffer size */
    uint32_t cs_pin: 30;                                            /**< CS pin */
    uint32_t cs_active: 2;                                          /**< CS active level */
};

/**
 * @addtogroup SPI.
 * @{
 */
int mr_spi_bus_register(struct mr_spi_bus *spi_bus, const char *name, struct mr_drv *drv);
int mr_spi_dev_register(struct mr_spi_dev *spi_dev, const char *name, int cs_pin, int cs_active);
/** @} */
#endif /* MR_USING_SPI */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MR_SPI_H_ */
