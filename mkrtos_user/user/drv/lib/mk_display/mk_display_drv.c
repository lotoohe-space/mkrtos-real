
#include "mk_display_drv.h"
#include "u_types.h"
#include <mk_access.h>
#include <assert.h>
#include <errno.h>
#include <mk_char_dev.h>
#include <u_thread.h>
#include <u_task.h>
#include <stdio.h>
/**
 * @brief 写pin操作
 *
 * @param pin pin设备
 * @param buf 写的buf
 * @param size 写入大小˝
 * @return int 写入的个数
 */
static int mk_display_drv_write(mk_dev_t *_dev, void *buf, size_t size)
{
    assert(buf);
    assert(_dev);

    return -ENOSYS;
}
/**
 * @brief 读取pin数据
 *
 * @param pin
 * @param buf
 * @param size
 * @return int
 */
static int mk_display_drv_read(mk_dev_t *_dev, void *buf, size_t size)
{
    assert(buf);
    assert(_dev);
    return -ENOSYS;
}

/**
 * @brief 控制pin状态
 *
 * @param pin
 * @param cmd
 * @param args
 * @return int
 */
static int mk_display_drv_ioctl(mk_dev_t *_dev, int cmd, umword_t args)
{
    assert(_dev);
    pid_t src_pid = thread_get_src_pid();
    mk_display_dev_t *dis_dev = _dev->priv_data;
    mk_display_ops_t *ops = dis_dev->ops;
    assert(ops);
    int ret;
    pid_t cur_pid;

    switch (cmd)
    {
    case MK_DISPLAY_SET_WIN:
    {
        msg_tag_t tag;
        mk_display_win_t dis_arg;

        tag = task_get_pid(TASK_THIS, (umword_t *)(&cur_pid));
        if (msg_tag_get_val(tag) < 0)
        {
            return msg_tag_get_val(tag);
        }
        // printf("%s:%d cmd:0x%x args:0x%x\n", __func__, __LINE__, cmd, args);
        ret = mk_copy_mem_to_task(src_pid, (void *)args,
                                  cur_pid, &dis_arg, sizeof(mk_display_win_t));
        if (ret < 0)
        {
            return ret;
        }
        if (ops->display_set_win)
        {
            ret = ops->display_set_win(&dis_dev->display, &dis_arg);
        }
        else
        {
            return -EIO;
        }
        return ret;
    }
    break;
    case MK_DISPLAY_GET_INFO:
    {
        mk_display_cfg_t cfg;
        mk_display_cfg_t *arg_cfg = (void *)args;
        msg_tag_t tag;

        tag = task_get_pid(TASK_THIS, (umword_t *)(&cur_pid));
        if (msg_tag_get_val(tag) < 0)
        {
            return msg_tag_get_val(tag);
        }
        if (ops->display_get_info)
        {
            ret = ops->display_get_info(&dis_dev->display, &cfg);
        }
        else
        {
            return -EIO;
        }
        if (ret >= 0)
        {
            ret = mk_copy_mem_to_task(cur_pid, &cfg, src_pid,
                                      arg_cfg, sizeof(mk_display_cfg_t));
            if (ret < 0)
            {
                return ret;
            }
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
    .write = NULL,
    .read = NULL,
    .ioctl = mk_display_drv_ioctl,
    .release = NULL,
};

int mk_display_register(mk_dev_t *dev, mk_display_dev_t *display_drv)
{
    int ret = 0;
    assert(dev);
    assert(display_drv);

    ret = mk_char_dev_register(dev, &file_ops);
    if (ret < 0)
    {
        return ret;
    }
    mk_dev_set_priv_data(dev, display_drv);

    return ret;
}