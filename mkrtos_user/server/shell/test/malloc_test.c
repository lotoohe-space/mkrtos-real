#include <malloc.h>
#include <string.h>

void malloc_test(void)
{
    void *mem = malloc(1024);
    assert(mem);
    void *mem1 = malloc(1024);
    assert(mem1);
    free(mem);
    free(mem1);
    mem = malloc(4 * 1024);
    assert(mem);
    free(mem);
    mem1 = malloc(1024);
    assert(mem1);
    free(mem1);
}
