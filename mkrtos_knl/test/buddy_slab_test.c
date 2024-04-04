
#include <buddy.h>
#include <slab.h>
#include <arch.h>
#include <assert.h>
#include <string.h>

void buddy_slab_init(void)
{
#define MEM_SIZE (128 * 1024)
    void *mem = buddy_alloc(buddy_get_alloter(), MEM_SIZE);

    assert(mem);
    memset(mem, 0, MEM_SIZE);
    buddy_free(buddy_get_alloter(), mem);
#undef MEM_SIZE
}
