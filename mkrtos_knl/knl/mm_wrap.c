#include "mm_wrap.h"
#include "types.h"
#include "mm.h"
#include "ram_limit.h"
#include "globals.h"

void *mm_limit_alloc(ram_limit_t *limit, size_t size)
{
    if (ram_limit_alloc(limit, size) == FALSE)
    {
        return NULL;
    }
    void *new_mem = mem_alloc(mm_get_global(), size + sizeof(size_t));

    if (!new_mem)
    {
        ram_limit_free(limit, size);
        return NULL;
    }
    *((size_t *)new_mem) = size;

    return (char *)new_mem + sizeof(size_t);
}
void mm_limit_free(ram_limit_t *limit, void *mem)
{
    size_t size = *((size_t *)(char *)mem - sizeof(size_t));

    mem_free(mm_get_global(), (char *)mem - sizeof(size_t));
    ram_limit_free(limit, size);
}