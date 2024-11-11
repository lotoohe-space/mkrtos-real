#pragma once

#include <u_types.h>
#include <u_slist.h>

struct mk_dev;
typedef struct mk_dev mk_dev_t;

typedef struct mk_drv_compatible
{
    const char *compatible;
} mk_drv_compatible_t;

typedef struct mk_drv
{
    mk_drv_compatible_t *compatilbe; /*数组，以NULL结尾 */
    int (*probe)(mk_dev_t *dev);
    slist_head_t node;
    int ref; //!< 引用计数
    void *data;
} mk_drv_t;

void mk_drv_init(void);
int mk_drv_register(mk_drv_t *drv);
int mk_drv_unregister(mk_drv_t *drv);
int mk_drv_mach_dev(int (*hook_mach)(mk_drv_t *drv, const char *dev_name, void *data),
                    const char *compat_name, const char *dev_name, void *data);
