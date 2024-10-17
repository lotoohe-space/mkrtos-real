/**
 * @file globals.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-09-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "globals.h"
#include "types.h"
#include "init.h"
#include "mm.h"
#include "kobject.h"
#include "prot.h"
#include "assert.h"
#include "mm_man.h"
#include "ipc.h"
#include <libfdt.h>
static mem_t global_mem;                     //!< 全局内存管理块
static kobject_t *kobj_ls[FACTORY_FUNC_MAX]; //!< 全局静态内核对象
/**
 * @brief 注册一个全局静态的内核对象
 *
 * @param kobj 注册的内核对象
 * @param inx 注册的内核对象号
 */
void global_reg_kobj(kobject_t *kobj, int inx)
{
    assert(inx >= 0);
    assert(inx < FACTORY_FUNC_MAX);
    kobj_ls[inx - 1] = kobj;
}
/**
 * @brief 获取注册的内核对象
 *
 * @param inx 获取哪一个
 * @return kobject_t* ==NULL 失败 !=NULL成功
 */
kobject_t *global_get_kobj(int inx)
{
    assert(inx >= 0);
    assert(inx < FACTORY_FUNC_MAX);
    return kobj_ls[inx - 1];
}

#if CONFIG_BUDDY_SLAB
#include <buddy.h>
#include <arch.h>
#endif
/**
 * @brief 系统内存初始化
 *
 */
static void mem_sys_init(void)
{

#if CONFIG_BUDDY_SLAB
    extern char _buddy_data_start[];
    // extern char _buddy_data_end[];
    int ret;
    size_t buddy_size = (size_t)CONFIG_SYS_DATA_SIZE - ((addr_t)_buddy_data_start - CONFIG_SYS_DATA_ADDR - CONFIG_BOOTSTRAP_TEXT_SIZE);

    ret = buddy_init(buddy_get_alloter(),
                     ALIGN((addr_t)_buddy_data_start, (1 << (BUDDY_MAX_ORDER + CONFIG_PAGE_SHIFT))) /*FIXME:这里可能会浪费一点内存*/,
                     ALIGN_DOWN(buddy_size - (1 << (BUDDY_MAX_ORDER + CONFIG_PAGE_SHIFT)), (1 << (BUDDY_MAX_ORDER + CONFIG_PAGE_SHIFT))));
    assert(ret >= 0);
    mmu_page_alloc_set(mm_buddy_alloc_one_page);
#else
    mm_init(arch_get_boot_info());
#endif
}
INIT_MEM(mem_sys_init);

#if IS_ENABLED(CONFIG_DTBO_SUPPORT)
static void dts_parse(void)
{
    printk("init dts parsing.\n");
    addr_t entry;
    size_t size;
    void *fdt;

    fdt = (void *)arch_get_boot_info()->flash_layer.flash_layer_list[DTBO_LAYER_1].st_addr;
    assert(fdt);

    int fdt_size = fdt_totalsize(fdt);
    printk("fdt_size:%d\n", fdt_size);

    fdt32_t magic = fdt_get_header(fdt, magic);
    assert(magic == 0xd00dfeed);
    fdt32_t version = fdt_get_header(fdt, version);
    printk("0x%x 0x%0x\n", magic, version);
    // #if !IS_ENABLED(CONFIG_MMU)
    //     dts_read_mem_node(fdt);
    //     dts_read_flash_node(fdt);
    // #endif
}
INIT_ONBOOT(dts_parse);
#endif
