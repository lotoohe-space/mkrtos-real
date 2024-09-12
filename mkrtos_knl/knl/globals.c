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
/**
 * @brief 获取内存分配对象
 *
 * @return mem_t*
 */
mem_t *mm_get_global(void)
{
    return &global_mem;
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
    void *mem_block_data;
    mem_init(&global_mem);
#if CONFIG_BUDDY_SLAB
    extern char _buddy_data_start[];
    // extern char _buddy_data_end[];
    int ret;
    size_t buddy_size = (size_t)CONFIG_KNL_DATA_SIZE - ((addr_t)_buddy_data_start - CONFIG_KNL_DATA_ADDR - CONFIG_KNL_OFFSET);

    ret = buddy_init(buddy_get_alloter(),
                     ALIGN((addr_t)_buddy_data_start, (1 << (BUDDY_MAX_ORDER + CONFIG_PAGE_SHIFT))) /*FIXME:这里可能会浪费一点内存*/,
                     ALIGN_DOWN(buddy_size - (1 << (BUDDY_MAX_ORDER + CONFIG_PAGE_SHIFT)), (1 << (BUDDY_MAX_ORDER + CONFIG_PAGE_SHIFT))));
    assert(ret >= 0);
    mmu_page_alloc_set(mm_buddy_alloc_one_page);
#else
#if CONFIG_KNL_EXRAM
    mem_heap_add(mm_get_global(), (void *)CONFIG_EX_RAM_ADDR, CONFIG_EX_RAM_SIZE);
#endif
    mem_heap_add(mm_get_global(), (void *)_ebss, CONFIG_KNL_DATA_SIZE - ((umword_t)_ebss - (umword_t)_sdata));
#endif
}
INIT_MEM(mem_sys_init);
