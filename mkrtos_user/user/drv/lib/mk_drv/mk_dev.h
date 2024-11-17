#pragma once

#include <u_slist.h>
#include "u_types.h"
#include <mk_drv.h>
#define MK_DEV_NAME_LEN 32
#define MK_COMPAT_NAME_LEN 32
typedef struct mk_dev
{
    char dev_name[MK_DEV_NAME_LEN];           //!< 设备名字
    char compatible_name[MK_COMPAT_NAME_LEN]; //!< compatible
    slist_head_t node;                        //!< 设备树列表节点
    int dtb_offset;                           //!< 设备树中的偏移
    mk_drv_t *drv;                            //!< 驱动节点
    int ref;                                  //!< 引用计数
    void *dtb;                                //!< dtb数据的起点
    void *priv_data;                          //!< 设备的私有数据
} mk_dev_t;

int mk_dev_init(void);
int mk_dev_register(mk_dev_t *dev, mk_drv_t *drv);
mk_dev_t *mk_dev_create(const char *name, const char *compatible_name, int dtb_offset, void *dtb);
static inline void mk_dev_set_priv_data(mk_dev_t *dev, void *priv_data)
{
    dev->priv_data = priv_data;
}
static inline char *mk_dev_get_dev_name(mk_dev_t *dev)
{
    return dev->dev_name;
}