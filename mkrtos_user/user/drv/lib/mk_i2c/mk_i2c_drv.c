
#include "mk_i2c_drv.h"
#include "u_types.h"
#include <assert.h>
#include <errno.h>
#include <mk_char_dev.h>
#include <sys/types.h>
/**
 * @brief 写pin操作
 *
 * @param pin pin设备
 * @param buf 写的buf
 * @param size 写入大小
 * @return int 写入的个数
 */
static int mk_i2c_drv_write(mk_dev_t *pin, void *buf, size_t size, off_t *offset)
{
    assert(buf);
    assert(pin);
    mk_i2c_dev_t *pin_dev = pin->priv_data;
    mk_i2c_ops_t *ops = pin_dev->ops;
    assert(ops);

    int ret = ops->i2c_bus_write(&pin_dev->i2c, buf, size, (uint16_t)((*offset) >> 16), (uint16_t)((*offset) & 0xff));

    return ret;
}
/**
 * @brief 读取pin数据
 *
 * @param pin
 * @param buf
 * @param size
 * @return int
 */
static int mk_i2c_drv_read(mk_dev_t *pin, void *buf, size_t size, off_t *offset)
{
    assert(buf);
    assert(pin);
    mk_i2c_dev_t *pin_dev = pin->priv_data;
    mk_i2c_ops_t *ops = pin_dev->ops;
    assert(ops);

    int ret = ops->i2c_bus_read(&pin_dev->i2c, buf, size, (uint16_t)((*offset) >> 16), (uint16_t)((*offset) & 0xff));

    return ret;
}
/**
 * @brief 控制pin状态
 *
 * @param pin
 * @param cmd
 * @param args
 * @return int
 */
static int mk_i2c_drv_ioctl(mk_dev_t *pin, int cmd, umword_t args)
{
    assert(pin);
    mk_i2c_dev_t *i2c_dev = pin->priv_data;
    mk_i2c_ops_t *ops = i2c_dev->ops;
    assert(ops);
    int ret = 0;

    switch (cmd)
    {
    case MK_I2C_SET_FREQ:
        ret = -ENOSYS;
        break;
    default:
        break;
    }
    return ret;
}
static mk_file_ops_t file_ops = {
    .open = NULL,
    .write = mk_i2c_drv_write,
    .read = mk_i2c_drv_read,
    .ioctl = mk_i2c_drv_ioctl,
    .release = NULL,
};

int mk_i2c_register(mk_dev_t *dev, mk_i2c_dev_t *pin_drv)
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