
#include "mk_dev.h"
#include <mk_drv.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <malloc.h>
#include <stdio.h>
#include "u_util.h"
static slist_head_t dev_list;

int mk_dev_init(void)
{
    slist_init(&dev_list);
    printf("%s init.\n", __func__);
    return 0;
}

mk_dev_t *mk_dev_create(const char *name, const char *compatible_name, int dtb_offset, void *dtb)
{
    mk_dev_t *dev = (mk_dev_t *)calloc(1, sizeof(mk_dev_t));

    if (!dev)
    {
        return NULL;
    }
    slist_init(&dev->node);
    strncpy(dev->dev_name, name, MK_DEV_NAME_LEN);
    dev->dev_name[MK_DEV_NAME_LEN - 1] = 0;

    strncpy(dev->compatible_name, compatible_name, MK_COMPAT_NAME_LEN);
    dev->compatible_name[MK_COMPAT_NAME_LEN - 1] = 0;
    dev->dtb_offset = dtb_offset;
    dev->dtb = dtb;

    return dev;
}

int mk_dev_register(mk_dev_t *dev, mk_drv_t *drv)
{
    assert(dev);
    mk_dev_t *pos;

    slist_foreach(pos, &dev_list, node)
    {
        if (strcmp(pos->dev_name, dev->dev_name) == 0)
        {
            return -EEXIST;
        }
    }
    slist_add(&dev_list, &dev->node);

    dev->drv = drv;
    drv->ref++;

    return 0;
}
