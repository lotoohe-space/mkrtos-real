
#include "types.h"
#include <task.h>
#include <mm_space.h>
#include <thread.h>

/**
 * @brief 检测内存是否可访问
 *
 * @param addr
 * @param size
 * @return bool_t
 */
bool_t is_rw_access(task_t *tg_task, void *addr, size_t size, bool_t ignore_null)
{
    if (tg_task == NULL)
    {
        return FALSE;
    }
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

    return FALSE;
}
