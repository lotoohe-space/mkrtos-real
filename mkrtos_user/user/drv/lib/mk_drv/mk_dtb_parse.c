
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <u_prot.h>
#include <u_sys.h>
#include <libfdt.h>
#include <mk_dev.h>
#include <mk_drv.h>
#include <stdio.h>
#include <u_vmam.h>
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
int dev_regs_map(mk_dev_t *dev, void *fdt)
{
    const umword_t *reg_raw;
    int len;
    msg_tag_t tag;

    reg_raw = fdt_getprop(dev->dtb, dev->dtb_offset,
                          "regs", &len);
    if (!reg_raw)
    {
        return -ENODEV;
    }
    if (len < sizeof(void *) * 2)
    {
        return -ENODEV;
    }

    /*寄存器两个一组*/
    for (int i = 0; i < len / sizeof(void *); i += 2)
    {
        addr_t vaddr;
        uint32_t reg[2];

        reg[0] = fdt32_to_cpu((fdt32_t)reg_raw[i]);
        reg[1] = fdt32_to_cpu((fdt32_t)reg_raw[i + 1]);

        printf("reg:0x%x size:0x%x\n", reg[0], reg[1]);
        tag = u_vmam_alloc(VMA_PROT, vma_addr_create(VPAGE_PROT_RWX, 0, 0),
                           reg[1], reg[0], &vaddr);
        if (msg_tag_get_val(tag) < 0)
        {
            if (i == 0)
            {
                printf("periph mem alloc failed..\n");
                return msg_tag_get_val(tag);
            }
            else
            {
                printf("periph mem alloc failed. success:%d\n", i / 2);
                return i / 2;
            }
        }
    }
    return 0;
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
