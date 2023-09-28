/**
 * @file globals.c
 * @author zhangzheng (1358745329@qq.com)
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
static uint8_t mem_block[58 * 1024];        //!< 内核内存分配堆 TODO:自动识别大小，或者从bootstrap中读取
static kobject_t *kobj_ls[FACTORY_FUNC_MAX]; //!< 全局静态内核对象

void global_reg_kobj(kobject_t *kobj, int inx)
{
    assert(inx >= 0);
    assert(inx < FACTORY_FUNC_MAX);
    kobj_ls[inx - 1] = kobj;
}
kobject_t *global_get_kobj(int inx)
{
    assert(inx >= 0);
    assert(inx < FACTORY_FUNC_MAX);
    return kobj_ls[inx - 1];
}

mem_t *mm_get_global(void)
{
    return &global_mem;
}
extern void log_dump(void);
extern void sys_dump(void);
static void mem_sys_init(void)
{
    log_dump();
    mm_man_dump();
    sys_dump();
    mem_init(&global_mem);
    mem_heap_add(mm_get_global(), mem_block, sizeof(mem_block));
}
INIT_MEM(mem_sys_init);
