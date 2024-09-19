#include "u_types.h"
#include "u_prot.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>
#if 0
void mm_test(void)
{
    void *mem = mm_alloc_page(MM_PROT, 2, REGION_RWX);
    assert(mem);
    memset((char *)mem, 0, 1024);
    void *mem1 = mm_alloc_page(MM_PROT, 2, REGION_RWX);
    assert(mem1);
    memset(mem1, 0, 1024);

    mm_free_page(MM_PROT, mem1, 2);
    mm_free_page(MM_PROT, mem, 2);
    printf("mm test success.\n");
}
#endif
