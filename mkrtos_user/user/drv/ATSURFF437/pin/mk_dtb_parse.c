
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <u_prot.h>
#include <u_sys.h>
#include <libfdt.h>
#include <mk_dev.h>
#include <mk_drv.h>
#include <stdio.h>
typedef struct hook_mach_info
{
    int node_offset;
    void *fdt;
    const char *compat_name;
} hook_mach_info_t;

static int dd_hook_mach(mk_drv_t *drv, const char *dev_name, void *data)
{
    mk_dev_t *dev;
    hook_mach_info_t *info = data;
    int ret = 0;

    printf("match dev succ, name is [%s]\n", dev_name);
    dev = mk_dev_create(dev_name, info->compat_name, info->node_offset, info->fdt);
    if (dev == NULL)
    {
        return -ENOMEM;
    }
    if (drv->probe)
    {
        ret = drv->probe(dev);
    }
    if (!ret)
    {
        ret = mk_dev_register(dev, drv);
    }
    return ret;
}

int dtb_parse_init(void)
{
    printf("init dts parsing.\n");

    addr_t entry;
    size_t size;
    void *fdt;

    fdt = (void *)sys_read_dtbo();
    assert(fdt);

    int fdt_size = fdt_totalsize(fdt);
    fdt32_t magic = fdt_get_header(fdt, magic);
    assert(magic == 0xd00dfeed);
    fdt32_t version = fdt_get_header(fdt, version);
    printf("fdt_size:%d 0x%x 0x%0x\n", fdt_size, magic, version);
    int root_node;
    int node_offset;

    root_node = fdt_path_offset(fdt, "/");
    if (root_node)
    {
        return -ENOENT;
    }
    fdt_for_each_subnode(node_offset, fdt, root_node)
    {
        const char *name;
        const char *compat_name;
        int len;
        hook_mach_info_t info;

        name = fdt_get_name(fdt, node_offset, &len);
        compat_name = fdt_getprop(fdt, node_offset, "compatible", &len);

        info.fdt = fdt;
        info.node_offset = node_offset;
        info.compat_name = compat_name;

        printf("dev name is [%s]\n", name);
        //! 匹配驱动
        if (!mk_drv_mach_dev(dd_hook_mach, compat_name, name, &info))
        {
            continue;
        }
    }

    return 0;
}
