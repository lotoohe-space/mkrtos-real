
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
bool_t is_rw_access(void *addr, size_t size, bool_t ignore_null)
{
    if (addr == NULL && ignore_null)
    {
        return TRUE;
    }

    void *mem;
    size_t mem_size;
    task_t *cur_task = thread_get_current_task();

    mm_space_get_ram_block(&cur_task->mm_space, &mem, &mem_size);
    if (mem >= addr && ((char *)addr + size) < (char *)mem + mem_size)
    {
        return TRUE;
    }

    return FALSE;
}
