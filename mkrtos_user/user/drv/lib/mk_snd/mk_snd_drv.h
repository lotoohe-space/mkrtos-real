#pragma once

#include <u_types.h>
#include <mk_drv.h>
#include <sys/types.h>


/**
 * @brief snd设备数据
 *
 */
typedef struct mk_snd
{
    void *priv_data;
} mk_snd_t;
/**
 * @brief snd设备操作对象
 *
 */
typedef struct mk_snd_ops
{
    int (*snd_configure)(mk_snd_t *drv);
    int (*snd_read)(mk_snd_t *drv, uint8_t *data, int len);
    int (*snd_write)(mk_snd_t *drv, uint8_t *dat, int len);
} mk_snd_ops_t;

/**
 * @brief snd设备
 *
 */
typedef struct mk_snd_dev
{
    mk_snd_t snd;
    mk_snd_ops_t *ops;
} mk_snd_dev_t;

int mk_snd_register(mk_dev_t *dev, mk_snd_dev_t *pin_drv);
