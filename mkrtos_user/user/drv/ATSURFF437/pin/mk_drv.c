

#include <mk_drv.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
static slist_head_t drv_list_head;

void mk_drv_init(void)
{
    slist_init(&drv_list_head);
    printf("%s init.\n", __func__);
}

static bool_t mk_drv_match(mk_drv_compatible_t *compat, const char *dev_name)
{
    while (compat && compat->compatible && dev_name)
    {
        if (strcmp(compat->compatible, dev_name) == 0)
        {
            return TRUE;
        }
        compat++;
    }
    return FALSE;
}

int mk_drv_mach_dev(int (*hook_mach)(mk_drv_t *drv, const char *dev_name, void *data),
                    const char *compat_name, const char *dev_name, void *data)
{
    assert(hook_mach);
    mk_drv_t *pos;

    slist_foreach(pos, &drv_list_head, node)
    {
        if (mk_drv_match(pos->compatilbe, compat_name) == TRUE)
        {
            return hook_mach(pos, dev_name, data);
        }
    }
    return -ENOENT;
}

int mk_drv_register(mk_drv_t *drv)
{
    slist_init(&drv->node);

    printf("register drv:%s\n", drv->compatilbe->compatible);
    slist_add(&drv_list_head, &drv->node);

    // TODO:循环匹配设备树(当前默认驱动必须比设备先注册)
    // TODO:检查驱动名字是否冲突

    return 0;
}
int mk_drv_unregister(mk_drv_t *drv)
{
    // TODO:取消注册，解除绑定
    return -1;
}
