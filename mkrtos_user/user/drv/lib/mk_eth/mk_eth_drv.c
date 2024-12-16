
#include "mk_eth_drv.h"
#include "u_types.h"
#include <assert.h>
#include <errno.h>
#include <mk_char_dev.h>
#include <sys/types.h>
#include <mk_access.h>
#include "u_prot.h"
#include "u_task.h"
/**
 * @brief 写pin操作
 *
 * @param pin pin设备
 * @param buf 写的buf
 * @param size 写入大小
 * @return int 写入的个数
 */
static int mk_eth_drv_write(mk_dev_t *pin, void *buf, size_t size, off_t *offset)
{
    assert(buf);
    assert(pin);
    mk_eth_dev_t *pin_dev = pin->priv_data;
    mk_eth_ops_t *ops = pin_dev->ops;
    assert(ops);

    int ret = ops->eth_write(&pin_dev->eth, buf, size);

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
static int mk_eth_drv_read(mk_dev_t *pin, void *buf, size_t size, off_t *offset)
{
    assert(buf);
    assert(pin);
    mk_eth_dev_t *pin_dev = pin->priv_data;
    mk_eth_ops_t *ops = pin_dev->ops;
    assert(ops);

    int ret = ops->eth_read(&pin_dev->eth, buf, size);

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
static int mk_eth_drv_ioctl(mk_dev_t *pin, int cmd, umword_t args)
{
    assert(pin);
    pid_t src_pid = thread_get_src_pid();
    pid_t cur_pid;
    mk_eth_dev_t *eth_dev = pin->priv_data;
    mk_eth_ops_t *ops = eth_dev->ops;
    assert(ops);
    int ret = 0;
    msg_tag_t tag;

    switch (cmd)
    {
    case ETH_SET_MAC_ADDR:
    {
        mk_ioctl_set_info_t info;

        tag = task_get_pid(TASK_THIS, (umword_t *)(&cur_pid));
        if (msg_tag_get_val(tag) < 0)
        {
            return msg_tag_get_val(tag);
        }
        ret = mk_copy_mem_to_task(src_pid, (void *)args,
                                  cur_pid, &info, sizeof(mk_ioctl_set_info_t));
        if (ret < 0)
        {
            return ret;
        }
        if (ops->eth_configure) {
            ops->eth_configure(eth_dev, cmd, &info);
        }
    }
    break;
    default:
        break;
    }
    return ret;
}
static mk_file_ops_t file_ops = {
    .open = NULL,
    .write = mk_eth_drv_write,
    .read = mk_eth_drv_read,
    .ioctl = mk_eth_drv_ioctl,
    .release = NULL,
};

int mk_eth_register(mk_dev_t *dev, mk_eth_dev_t *pin_drv)
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