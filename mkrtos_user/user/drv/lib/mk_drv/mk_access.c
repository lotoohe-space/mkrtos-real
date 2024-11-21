

#include "mk_access.h"
#include <pm_cli.h>

int mk_copy_mem_to_task(pid_t src_pid, void *src, pid_t dst_pid, void *dst, size_t len)
{
    return pm_copy_data(src_pid, dst_pid, (addr_t)src, (addr_t)dst, len);
}
