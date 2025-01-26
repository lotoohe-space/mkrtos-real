/**
 * @file access.c
 * @author zhangzheng (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-12-05
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "types.h"
#include <task.h>
#include <mm_space.h>
#include <thread.h>
#include <util.h>
/**
 * @brief 检测内存是否可访问
 *
 * @param tg_task 目标进程
 * @param addr 开始地址
 * @param size 大小
 * @param ignore_null 是否忽略nullptr
 * @return bool_t TRUE：可访问，FALSE：不可访问
 */
bool_t is_rw_access(task_t *tg_task, void *addr, size_t size, bool_t ignore_null)
{
    if (tg_task == NULL)
    {
        return FALSE;
    }
#if !IS_ENABLED(CONFIG_MMU)
    if (addr == NULL && ignore_null)
    {
        return TRUE;
    }

    void *mem;
    size_t mem_size;

    mm_space_get_ram_block(&tg_task->mm_space, &mem, &mem_size);
    if (mem <= addr && (((char *)addr + size) <= ((char *)mem + mem_size)))
    {
        return TRUE;
    }
#if IS_ENABLED(CONFIG_MMU)
/*TODO:*/
#endif
    return FALSE;
#else
    return TRUE;
#endif
}
