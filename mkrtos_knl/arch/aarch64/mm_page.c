#include <buddy.h>
#include <asm/mm.h>
#include "mm_page.h"
#include <mmu.h>
#include <string.h>
int page_entry_init(page_entry_t *entry)
{
    entry->dir = buddy_alloc(buddy_get_alloter(), PAGE_SIZE);

    if (entry->dir == NULL)
    {
        return -ENOMEM;
    }
    memset(entry->dir, 0, PAGE_SIZE);
    knl_pdir_init(entry, entry->dir, 3);
    return 0;
}
