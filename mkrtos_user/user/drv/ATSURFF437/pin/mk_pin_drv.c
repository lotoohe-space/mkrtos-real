
#include "mk_pin_drv.h"
#include "u_types.h"
#include <assert.h>
#include <errno.h>
#include <mk_char_dev.h>

/**
 * @brief 写pin操作
 *
 * @param pin pin设备
 * @param buf 写的buf
 * @param size 写入大小
 * @return int 写入的个数
 */
static int mk_pin_drv_write(mk_dev_t *pin, void *buf, size_t size)
{
    assert(buf);
    assert(pin);
    uint8_t *_buf = buf;
    mk_pin_dev_t *pin_dev = pin->priv_data;
    mk_pin_ops_t *ops = pin_dev->ops;
    assert(ops);
    int i;

    for (i = 0; i < size; i++)
    {
        int ret = ops->drv_pin_write(&pin_dev->pins, pin_dev->val + i, _buf[i]);

        if (ret < 0)
        {
            if (i == 0)
            {
                return ret;
            }
            return i;
        }
    }

    return i;
}
/**
 * @brief 读取pin数据
 *
 * @param pin
 * @param buf
 * @param size
 * @return int
 */
static int mk_pin_drv_read(mk_dev_t *pin, void *buf, size_t size)
{
    assert(buf);
    assert(pin);
    uint8_t *_buf = buf;
    mk_pin_dev_t *pin_dev = pin->priv_data;
    mk_pin_ops_t *ops = pin_dev->ops;
    assert(ops);
    int i;

    for (i = 0; i < size; i++)
    {
        uint8_t rval = 0;
        int ret = ops->drv_pin_read(&pin_dev->pins, pin_dev->val + i, &rval);

        if (ret < 0)
        {
            if (i == 0)
            {
                return ret;
            }
            return i;
        }
        _buf[i] = (uint8_t)rval;
    }

    return i;
}
/**
 * @brief 控制pin状态
 *
 * @param pin
 * @param cmd
 * @param args
 * @return int
 */
static int mk_pin_drv_ioctl(mk_dev_t *pin, int cmd, umword_t args)
{
    assert(pin);
    mk_pin_dev_t *pin_dev = pin->priv_data;
    mk_pin_ops_t *ops = pin_dev->ops;
    assert(ops);
    int ret;

    switch (cmd)
    {
    case MK_PIN_SET_MODE:
    {
        mk_pin_cfg_t cfg = (mk_pin_cfg_t){.cfg_raw = args};

        if (cfg.pin >= sizeof(pin_dev->pins))
        {
            return -EINVAL;
        }
        if (ops->drv_pin_configure)
        {
            ret = ops->drv_pin_configure(&pin_dev->pins, cfg.pin, cfg.mode);
            if (ret < 0)
            {
                return ret;
            }
        }
        else
        {
            return -EIO;
        }
    }
    break;
    case MK_PIN_GET_MODE:
    {
        mk_pin_cfg_t cfg = (mk_pin_cfg_t){.cfg_raw = args};

        if (cfg.pin >= sizeof(pin_dev->pins))
        {
            return -EINVAL;
        }
        return pin_dev->pins.pins[cfg.pin];
    }
    break;
    case MK_PIN_SET_OP_PIN:
    {
        if (args > sizeof(pin_dev->pins))
        {
            return -EINVAL;
        }
        pin_dev->val = (args);
    }
    break;
    default:
        break;
    }
    return 0;
}
static mk_file_ops_t file_ops = {
    .mk_pin_drv_open = NULL,
    .mk_pin_drv_write = mk_pin_drv_write,
    .mk_pin_drv_read = mk_pin_drv_read,
    .mk_pin_drv_ioctl = mk_pin_drv_ioctl,
    .mk_pin_drv_release = NULL,
};

int mk_pin_register(mk_dev_t *dev, mk_pin_dev_t *pin_drv)
{
    int ret = 0;
    assert(dev);
    assert(pin_drv);

    ret = mk_char_dev_register(dev, &file_ops);
    if (ret < 0)
    {
        return ret;
    }
    mk_dev_set_priv_data(dev, pin_drv);

    return ret;
}