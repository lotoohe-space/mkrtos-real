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
static mem_t global_mem;                                 //!< 全局内存管理块
static uint8_t mem_block[9 * 1024 * 1024 + 1024 * 1000]; //!< 内核内存分配堆 TODO:自动识别大小，或者从bootstrap中读取
static kobject_t *kobj_ls[FACTORY_FUNC_MAX];             //!< 全局静态内核对象
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
/**
 * @brief 系统内存初始化
 *
 */
static void mem_sys_init(void)
{
    mem_init(&global_mem);
    mem_heap_add(mm_get_global(), mem_block, sizeof(mem_block));
}
INIT_MEM(mem_sys_init);
