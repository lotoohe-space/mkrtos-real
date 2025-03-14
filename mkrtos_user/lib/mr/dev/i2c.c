/*
 * @copyright (c) 2023, MR Development Team
 *
 * @license SPDX-License-Identifier: Apache-2.0
 *
 * @date 2023-11-09    MacRsh       First version
 */

#include "i2c.h"

#ifdef MR_USING_I2C

#define MR_I2C_RD                       (0)
#define MR_I2C_WR                       (1)

MR_INLINE void i2c_bus_send_addr(struct mr_i2c_bus *i2c_bus, int rdwr)
{
    struct mr_i2c_bus_ops *ops = (struct mr_i2c_bus_ops *)i2c_bus->dev.drv->ops;
    uint8_t addr = 0;

    if (i2c_bus->owner != MR_NULL)
    {
        addr = (((struct mr_i2c_dev *)i2c_bus->owner)->addr) << 1;
    }

    ops->start(i2c_bus);
    if (rdwr == MR_I2C_WR)
    {
        ops->write(i2c_bus, &addr, sizeof(addr));
    } else
    {
        addr |= 0x01;
        ops->write(i2c_bus, &addr, sizeof(addr));
    }
}

static int mr_i2c_bus_open(struct mr_dev *dev)
{
    struct mr_i2c_bus *i2c_bus = (struct mr_i2c_bus *)dev;
    struct mr_i2c_bus_ops *ops = (struct mr_i2c_bus_ops *)dev->drv->ops;

    return ops->configure(i2c_bus, &i2c_bus->config);
}

static int mr_i2c_bus_close(struct mr_dev *dev)
{
    struct mr_i2c_bus *i2c_bus = (struct mr_i2c_bus *)dev;
    struct mr_i2c_bus_ops *ops = (struct mr_i2c_bus_ops *)dev->drv->ops;
    struct mr_i2c_config close_config = {0};

    return ops->configure(i2c_bus, &close_config);
}

static ssize_t mr_i2c_bus_read(struct mr_dev *dev, int off, void *buf, size_t size, int sync_or_async)
{
    struct mr_i2c_bus *i2c_bus = (struct mr_i2c_bus *)dev;
    struct mr_i2c_bus_ops *ops = (struct mr_i2c_bus_ops *)i2c_bus->dev.drv->ops;

    if (i2c_bus->config.host_slave == MR_I2C_HOST)
    {
        if (off >= 0)
        {
            i2c_bus_send_addr(i2c_bus, MR_I2C_WR);
            ops->write(i2c_bus, &off, i2c_bus->config.off_bits);
        }

        i2c_bus_send_addr(i2c_bus, MR_I2C_RD);
        ssize_t ret = ops->read(i2c_bus, buf, size);
        ops->stop(i2c_bus);
        return ret;
    } else
    {
        return ops->read(i2c_bus, buf, size);
    }
}

static ssize_t mr_i2c_bus_write(struct mr_dev *dev, int off, const void *buf, size_t size, int sync_or_async)
{
    struct mr_i2c_bus *i2c_bus = (struct mr_i2c_bus *)dev;
    struct mr_i2c_bus_ops *ops = (struct mr_i2c_bus_ops *)i2c_bus->dev.drv->ops;

    if (i2c_bus->config.host_slave == MR_I2C_HOST)
    {
        i2c_bus_send_addr(i2c_bus, MR_I2C_WR);
        if (off >= 0)
        {
            ops->write(i2c_bus, &off, i2c_bus->config.off_bits);
        }

        ssize_t ret = ops->write(i2c_bus, buf, size);
        ops->stop(i2c_bus);
        return ret;
    } else
    {
        return ops->write(i2c_bus, buf, size);
    }
}

static int mr_i2c_bus_ioctl(struct mr_dev *dev, int off, int cmd, void *args)
{
    struct mr_i2c_bus *i2c_bus = (struct mr_i2c_bus *)dev;
    struct mr_i2c_bus_ops *ops = (struct mr_i2c_bus_ops *)i2c_bus->dev.drv->ops;

    switch (cmd)
    {
        case MR_IOCTL_SET_CONFIG:
        {
            if (args != MR_NULL)
            {
                struct mr_i2c_config *config = (struct mr_i2c_config *)args;

                if (i2c_bus->lock > 0)
                {
                    return MR_EBUSY;
                }

                if (config->host_slave == MR_I2C_HOST)
                {
                    int ret = ops->configure(i2c_bus, config);
                    if (ret == MR_EOK)
                    {
                        i2c_bus->config = *config;
                    }
                    i2c_bus->owner = MR_NULL;
                    return MR_EOK;
                }
                return MR_EINVAL;
            }
            return MR_EINVAL;
        }

        case MR_IOCTL_GET_CONFIG:
        {
            if (args != MR_NULL)
            {
                struct mr_i2c_config *config = (struct mr_i2c_config *)args;

                *config = i2c_bus->config;
                return MR_EOK;
            }
            return MR_EINVAL;
        }

        default:
        {
            return MR_ENOTSUP;
        }
    }
}

static ssize_t mr_i2c_bus_isr(struct mr_dev *dev, int event, void *args)
{
    struct mr_i2c_bus *i2c_bus = (struct mr_i2c_bus *)dev;
    struct mr_i2c_bus_ops *ops = (struct mr_i2c_bus_ops *)i2c_bus->dev.drv->ops;

    switch (event)
    {
        case MR_ISR_EVENT_RD_INTER:
        {
            struct mr_i2c_dev *i2c_dev = (struct mr_i2c_dev *)i2c_bus->owner;
            uint8_t data = 0;

            ops->read(i2c_bus, &data, sizeof(data));
            mr_ringbuf_push_force(&i2c_dev->rd_fifo, data);
            if (i2c_dev->dev.rd_cb.cb != MR_NULL)
            {
                size_t size = (ssize_t)mr_ringbuf_get_data_size(&i2c_dev->rd_fifo);
                i2c_dev->dev.rd_cb.cb(i2c_dev->dev.rd_cb.desc, &size);
            }
            return (ssize_t)mr_ringbuf_get_data_size(&i2c_dev->rd_fifo);
        }

        default:
        {
            return MR_ENOTSUP;
        }
    }
}

/**
 * @brief This function registers a i2c-bus.
 *
 * @param i2c_bus The i2c-bus.
 * @param name The name of the i2c-bus.
 * @param drv The driver of the i2c-bus.
 *
 * @return MR_EOK on success, otherwise an error code.
 */
int mr_i2c_bus_register(struct mr_i2c_bus *i2c_bus, const char *name, struct mr_drv *drv)
{
    static struct mr_dev_ops ops =
        {
            mr_i2c_bus_open,
            mr_i2c_bus_close,
            mr_i2c_bus_read,
            mr_i2c_bus_write,
            mr_i2c_bus_ioctl,
            mr_i2c_bus_isr
        };
    struct mr_i2c_config default_config = MR_I2C_CONFIG_DEFAULT;

    mr_assert(i2c_bus != MR_NULL);
    mr_assert(name != MR_NULL);
    mr_assert(drv != MR_NULL);
    mr_assert(drv->ops != MR_NULL);

    /* Initialize the fields */
    i2c_bus->config = default_config;
    i2c_bus->lock = 0;
    i2c_bus->owner = MR_NULL;

    /* Register the i2c-bus */
    return mr_dev_register(&i2c_bus->dev, name, Mr_Dev_Type_I2c, MR_SFLAG_RDWR, &ops, drv);
}

MR_INLINE int i2c_dev_take_bus(struct mr_i2c_dev *i2c_dev)
{
    struct mr_i2c_bus *i2c_bus = (struct mr_i2c_bus *)i2c_dev->dev.link;
    struct mr_i2c_bus_ops *ops = (struct mr_i2c_bus_ops *)i2c_bus->dev.drv->ops;

    if ((i2c_dev != i2c_bus->owner) && (i2c_bus->owner != MR_NULL))
    {
        return MR_EBUSY;
    }

    if (i2c_bus->owner == MR_NULL)
    {
        if (i2c_dev->config.baud_rate != i2c_bus->config.baud_rate
            || i2c_dev->config.host_slave != i2c_bus->config.host_slave)
        {
            int ret = ops->configure(i2c_bus, &i2c_dev->config);
            if (ret != MR_EOK)
            {
                i2c_bus->lock--;
                return ret;
            }
        }
        i2c_bus->config = i2c_dev->config;
        i2c_bus->owner = i2c_dev;
        i2c_bus->lock = 0;
    }
    i2c_bus->lock++;
    return MR_EOK;
}

MR_INLINE int i2c_dev_release_bus(struct mr_i2c_dev *i2c_dev)
{
    struct mr_i2c_bus *i2c_bus = (struct mr_i2c_bus *)i2c_dev->dev.link;

    if (i2c_dev != i2c_bus->owner)
    {
        return MR_EINVAL;
    }

    i2c_bus->lock--;
    if (i2c_bus->lock == 0)
    {
        i2c_bus->owner = MR_NULL;
    }
    return MR_EOK;
}

static int mr_i2c_dev_open(struct mr_dev *dev)
{
    struct mr_i2c_dev *i2c_dev = (struct mr_i2c_dev *)dev;

    /* Allocate FIFO buffers */
    return mr_ringbuf_allocate(&i2c_dev->rd_fifo, i2c_dev->rd_bufsz);
}

static int mr_i2c_dev_close(struct mr_dev *dev)
{
    struct mr_i2c_dev *i2c_dev = (struct mr_i2c_dev *)dev;

    /* Free FIFO buffers */
    mr_ringbuf_free(&i2c_dev->rd_fifo);
    return MR_EOK;
}

static ssize_t mr_i2c_dev_read(struct mr_dev *dev, int off, void *buf, size_t size, int sync_or_async)
{
    struct mr_i2c_dev *i2c_dev = (struct mr_i2c_dev *)dev;

    ssize_t ret = i2c_dev_take_bus(i2c_dev);
    if (ret != MR_EOK)
    {
        return ret;
    }

    if (i2c_dev->config.host_slave == MR_I2C_HOST)
    {
        ret = mr_i2c_bus_read(dev->link, off, buf, size, sync_or_async);
    } else
    {
        if (mr_ringbuf_get_bufsz(&i2c_dev->rd_fifo) == 0)
        {
            ret = mr_i2c_bus_read(dev->link, -1, buf, size, sync_or_async);
        } else
        {
            ret = (ssize_t)mr_ringbuf_read(&i2c_dev->rd_fifo, buf, size);
        }
    }

    i2c_dev_release_bus(i2c_dev);
    return ret;
}

static ssize_t mr_i2c_dev_write(struct mr_dev *dev, int off, const void *buf, size_t size, int sync_or_async)
{
    struct mr_i2c_dev *i2c_dev = (struct mr_i2c_dev *)dev;

    ssize_t ret = i2c_dev_take_bus(i2c_dev);
    if (ret != MR_EOK)
    {
        return ret;
    }

    if (i2c_dev->config.host_slave == MR_I2C_HOST)
    {
        ret = mr_i2c_bus_write(dev->link, off, buf, size, sync_or_async);
    } else
    {
        ret = mr_i2c_bus_write(dev->link, -1, buf, size, sync_or_async);
    }

    i2c_dev_release_bus(i2c_dev);
    return ret;
}

static int mr_i2c_dev_ioctl(struct mr_dev *dev, int off, int cmd, void *args)
{
    struct mr_i2c_dev *i2c_dev = (struct mr_i2c_dev *)dev;

    switch (cmd)
    {
        case MR_IOCTL_SET_CONFIG:
        {
            if (args != MR_NULL)
            {
                struct mr_i2c_bus *i2c_bus = (struct mr_i2c_bus *)dev->link;
                struct mr_i2c_config *config = (struct mr_i2c_config *)args;

                i2c_dev->config = *config;
                if (i2c_dev == i2c_bus->owner)
                {
                    i2c_bus->owner = MR_NULL;
                }
                if (config->host_slave == MR_I2C_SLAVE)
                {
                    int ret = i2c_dev_take_bus(i2c_dev);
                    if (ret != MR_EOK)
                    {
                        return ret;
                    }
                }
                return MR_EOK;
            }
            return MR_EINVAL;
        }
        case MR_IOCTL_SET_RD_BUFSZ:
        {
            if (args != MR_NULL)
            {
                size_t bufsz = *(size_t *)args;

                int ret = mr_ringbuf_allocate(&i2c_dev->rd_fifo, bufsz);
                i2c_dev->rd_bufsz = 0;
                if (ret == MR_EOK)
                {
                    i2c_dev->rd_bufsz = bufsz;
                }
                return ret;
            }
            return MR_EINVAL;
        }

        case MR_IOCTL_GET_CONFIG:
        {
            if (args != MR_NULL)
            {
                struct mr_i2c_config *config = (struct mr_i2c_config *)args;

                *config = i2c_dev->config;
                return MR_EOK;
            }
            return MR_EINVAL;
        }
        case MR_IOCTL_GET_RD_BUFSZ:
        {
            if (args != MR_NULL)
            {
                *(size_t *)args = i2c_dev->rd_bufsz;
                return MR_EOK;
            }
            return MR_EINVAL;
        }

        default:
        {
            return MR_ENOTSUP;
        }
    }
}

/**
 * @brief This function registers a i2c-device.
 *
 * @param i2c_dev The i2c-device.
 * @param name The name of the i2c-device.
 * @param addr The address of the i2c-device.
 * @param addr_bits The number of address bits of the i2c-device.
 *
 * @return MR_EOK on success, otherwise an error code.
 */
int mr_i2c_dev_register(struct mr_i2c_dev *i2c_dev, const char *name, int addr, int addr_bits)
{
    static struct mr_dev_ops ops =
        {
            mr_i2c_dev_open,
            mr_i2c_dev_close,
            mr_i2c_dev_read,
            mr_i2c_dev_write,
            mr_i2c_dev_ioctl
        };
    struct mr_i2c_config default_config = MR_I2C_CONFIG_DEFAULT;

    mr_assert(i2c_dev != MR_NULL);
    mr_assert(name != MR_NULL);
    mr_assert((addr_bits == MR_I2C_ADDR_BITS_7) || (addr_bits == MR_I2C_ADDR_BITS_10));

    /* Initialize the fields */
    i2c_dev->config = default_config;
    mr_ringbuf_init(&i2c_dev->rd_fifo, MR_NULL, 0);
#ifndef MR_CFG_I2C_RD_BUFSZ_INIT
#define MR_CFG_I2C_RD_BUFSZ_INIT        (0)
#endif /* MR_CFG_I2C_RD_BUFSZ_INIT */
    i2c_dev->rd_bufsz = MR_CFG_I2C_RD_BUFSZ_INIT;
    i2c_dev->addr = addr;
    i2c_dev->addr_bits = addr_bits;

    /* Register the i2c-device */
    return mr_dev_register(&i2c_dev->dev, name, Mr_Dev_Type_I2c, MR_SFLAG_RDWR | MR_SFLAG_NONDRV, &ops, MR_NULL);
}

#endif /* MR_USING_I2C */
