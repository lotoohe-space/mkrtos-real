#include <rtthread.h>
#include <malloc.h>

rt_weak void *rt_calloc(rt_size_t count, rt_size_t size)
{
    return calloc(count, size);
}
