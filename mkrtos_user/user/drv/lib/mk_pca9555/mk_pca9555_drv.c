
#include "mk_pca9555_drv.h"
#include "u_types.h"
#include <assert.h>
#include <errno.h>
#include <mk_char_dev.h>

/**
 * @brief 写pca9555操作
 *
 * @param pin pin设备
 * @param buf 写的buf
 * @param size 写入大小
 * @return int 写入的个数
 */
static int mk_pca9555_drv_write(mk_dev_t *pin, void *buf, size_t size, off_t *offset)
{
    assert(buf);
    assert(pin);
    mk_pca9555_dev_t *pin_dev = pin->priv_data;
    mk_pca9555_ops_t *ops = pin_dev->ops;
    assert(ops);

    int ret = ops->pca9555_write(&pin_dev->pca9555, buf, size);

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
static int mk_pca9555_drv_read(mk_dev_t *pin, void *buf, size_t size, off_t *offset)
{
    assert(buf);
    assert(pin);
    mk_pca9555_dev_t *pin_dev = pin->priv_data;
    mk_pca9555_ops_t *ops = pin_dev->ops;
    assert(ops);

    int ret = ops->pca9555_read(&pin_dev->pca9555, buf, size);

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
static int mk_pca9555_drv_ioctl(mk_dev_t *pin, int cmd, umword_t args)
{
    assert(pin);
    mk_pca9555_dev_t *pca9555_dev = pin->priv_data;
    mk_pca9555_ops_t *ops = pca9555_dev->ops;
    assert(ops);
    int ret = 0;

    switch (cmd)
    {
    case PCA9555_SET_INPUT_MODE:
    case PCA9555_SET_OUTPUT_MODE:
    case PCA9555_IO_RESET:
    case PCA9555_IO_SET:
        if (ops->pca9555_configure)
        {
            ret = ops->pca9555_configure(&pca9555_dev->pca9555, cmd, args);
        }
        break;
    default:
        break;
    }
    return ret;
}
static mk_file_ops_t file_ops = {
    .open = NULL,
    .write = mk_pca9555_drv_write,
    .read = mk_pca9555_drv_read,
    .ioctl = mk_pca9555_drv_ioctl,
    .release = NULL,
};

int mk_pca9555_register(mk_dev_t *dev, mk_pca9555_dev_t *pin_drv)
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