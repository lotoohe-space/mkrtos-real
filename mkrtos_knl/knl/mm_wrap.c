/**
 * @file mm_wrap.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-09-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "mm_wrap.h"
#include "types.h"
#include "mm.h"
#include "ram_limit.h"
#include "globals.h"
#include "string.h"
#include "boot_info.h"
static mem_t mem_manage[MEM_NODES_NUM];

void mm_init(boot_info_t *info)
{
    for (int i = 0; i < info->mem.mem_num; i++)
    {
        mem_init(&mem_manage[i]);
        if ((addr_t)_ebss >= info->mem.mem_list[i].addr && (addr_t)_ebss < info->mem.mem_list[i].addr + info->mem.mem_list[i].size)
        {
            /*该内存与系统共用，需要吧系统用到的内存扣掉*/
            mem_heap_add(&mem_manage[i], (void *)_ebss, info->mem.mem_list[i].size - ((umword_t)_ebss - (umword_t)_sdata));
        }
        else
        {
            mem_heap_add(&mem_manage[i], (void *)info->mem.mem_list[i].addr, info->mem.mem_list[i].size);
        }
    }
}
void *mm_limit_alloc_raw(int mem_inx, ram_limit_t *limit, size_t size)
{
    if (mem_inx >= MEM_NODES_NUM)
    {
        return NULL;
    }
    if (ram_limit_alloc(limit, size) == FALSE)
    {
        return NULL;
    }
    void *new_mem = mem_alloc(&mem_manage[mem_inx], size + sizeof(size_t));

    if (!new_mem)
    {
        ram_limit_free(limit, size);
        return NULL;
    }
    memset(new_mem, 0, size + sizeof(size_t));
    *((size_t *)new_mem) = size;

    return (char *)new_mem + sizeof(size_t);
}
void mm_limit_free_raw(int mem_inx, ram_limit_t *limit, void *mem)
{
    if (mem_inx >= MEM_NODES_NUM)
    {
        return;
    }
    if (!mem)
    {
        return;
    }
    size_t size = *((size_t *)((char *)mem - sizeof(size_t)));

    mem_free(&mem_manage[mem_inx], (char *)mem - sizeof(size_t));
    ram_limit_free(limit, size);
}
void *mm_limit_alloc(ram_limit_t *limit, size_t size)
{
    if (ram_limit_alloc(limit, size) == FALSE)
    {
        return NULL;
    }
    void *new_mem = mem_alloc(&mem_manage[0], size + sizeof(size_t));

    if (!new_mem)
    {
        ram_limit_free(limit, size);
        return NULL;
    }
    memset(new_mem, 0, size + sizeof(size_t));
    *((size_t *)new_mem) = size;

    return (char *)new_mem + sizeof(size_t);
}
void mm_limit_free(ram_limit_t *limit, void *mem)
{
    if (!mem)
    {
        return;
    }
    size_t size = *((size_t *)((char *)mem - sizeof(size_t)));

    mem_free(&mem_manage[0], (char *)mem - sizeof(size_t));
    ram_limit_free(limit, size);
}
struct mem_heap *mm_get_free(struct mem_heap *next,
                             umword_t hope_size, umword_t *ret_addr)
{
    return mem_get_free(&mem_manage[0], next, hope_size, (uint32_t *)ret_addr);
}
void mm_trace(void)
{
    mem_trace(&mem_manage[0]);
}
void mm_info(size_t *total, size_t *free)
{
    mem_info(&mem_manage[0], total, free);
}
void *mm_limit_alloc_align(ram_limit_t *limit, size_t size, size_t align)
{
    if (ram_limit_alloc(limit, size) == FALSE)
    {
        return NULL;
    }
    void *new_mem = mem_alloc_align(&mem_manage[0], size, align);

    if (!new_mem)
    {
        ram_limit_free(limit, size);
        return NULL;
    }

    return (char *)new_mem;
}
void mm_limit_free_align(ram_limit_t *limit, void *mem, size_t size)
{
    if (!mem)
    {
        return;
    }
    mem_free_align(&mem_manage[0], (char *)mem);
    ram_limit_free(limit, size);
}
#if IS_ENABLED(CONFIG_BUDDY_SLAB)
#include <buddy.h>
#include <slab.h>
void *mm_buddy_alloc_one_page(void)
{
    void *mem = buddy_alloc(buddy_get_alloter(), PAGE_SIZE);

    if (mem)
    {
        memset(mem, 0, PAGE_SIZE);
    }
    return mem;
}
void *mm_limit_alloc_buddy(ram_limit_t *limit, size_t size)
{
    if (ram_limit_alloc(limit, size) == FALSE)
    {
        return NULL;
    }
    void *new_mem = buddy_alloc(buddy_get_alloter(), size);

    if (!new_mem)
    {
        ram_limit_free(limit, size);
        return NULL;
    }

    return (char *)new_mem;
}
void mm_limit_free_buddy(ram_limit_t *limit, void *mem, size_t size)
{
    if (!mem)
    {
        return;
    }
    buddy_free(buddy_get_alloter(), (char *)mem);
    ram_limit_free(limit, size);
}
void *mm_limit_alloc_slab(slab_t *slab, ram_limit_t *limit)
{
    if (ram_limit_alloc(limit, slab_get_item_size(slab)) == FALSE)
    {
        return NULL;
    }
    void *new_mem = slab_alloc(slab);

    if (!new_mem)
    {
        ram_limit_free(limit, slab_get_item_size(slab));
        return NULL;
    }

    return (char *)new_mem;
}
void mm_limit_free_slab(slab_t *slab, ram_limit_t *limit, void *mem)
{
    if (!mem)
    {
        return;
    }
    slab_free(slab, (char *)mem);
    ram_limit_free(limit, slab_get_item_size(slab));
}
#endif