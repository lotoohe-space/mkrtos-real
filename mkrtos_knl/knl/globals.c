/*
 * @Author: zhangzheng
 * @Date: 2023-08-14 09:47:54
 * @LastEditors: zhangzheng 1358745329@qq.com
 * @FilePath: /mkrtos-real/mkrtos_knl/inc/knl/globals.h
 * @Description:
 */
#include "globals.h"
#include "types.h"
#include "init.h"
#include "mm.h"
#include "kobject.h"
#include "prot.h"
#include "assert.h"

static mem_t global_mem;                     //!< 全局内存管理块
static uint8_t mem_block[8 * 1024];          //!< 内核内存分配堆
static kobject_t *kobj_ls[FACTORY_FUNC_MAX]; //!< 全局静态内核对象

void global_reg_kobj(kobject_t *kobj, int inx)
{
    assert(inx >= 0);
    assert(inx < FACTORY_FUNC_MAX);
    kobj_ls[inx - 1] = kobj;
}

mem_t *mm_get_global(void)
{
    return &global_mem;
}

INIT_MEM static void mem_sys_init(umword_t st_addr, size_t size)
{
    mem_init(&global_mem);
    mem_heap_add(mm_get_global(), mem_block, sizeof(mem_block));
}