#include <malloc.h>
#include <string.h>
#include <assert.h>
void malloc_test(void)
{
#define TEST_MEM_SIZE 1024 * 1024 * 4
    void *mem2 = malloc(TEST_MEM_SIZE);
    assert(mem2);
    memset(mem2, 0, TEST_MEM_SIZE);
    free(mem2);
#undef TEST_MEM_SIZE

    void *mem[1000];
    for (int i = 0; i < 1000; i++)
    {
        mem[i] = malloc(4096);
        assert(mem[i]);
        memset(mem, 0, 4096);
    }
    for (int i = 0; i < 1000; i++)
    {
        free(mem[i]);
    }
}
