#include <malloc.h>
#include <string.h>
#include <assert.h>
void malloc_test(void)
{
#define TEST_MEM_SIZE 1024 * 1024 * 32
    void *mem = malloc(TEST_MEM_SIZE);
    assert(mem);
    memset(mem, 0, TEST_MEM_SIZE);

    free(mem);
#undef TEST_MEM_SIZE
}
