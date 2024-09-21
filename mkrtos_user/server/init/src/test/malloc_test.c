#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <CuTest.h>
#include <stdio.h>
#include <u_types.h>
static uint8_t *mem[1000];
static void malloc_test(CuTest *cu)
{
#define TEST_MEM_SIZE (MK_PAGE_SIZE)
    void *mem2 = malloc(TEST_MEM_SIZE);
    printf("alloc mem 0x%lx\n", mem2);

    CuAssert(cu, "malloc failed\n", mem2 != NULL);
    memset(mem2, 0, TEST_MEM_SIZE);
    free(mem2);
    printf("free mem 0x%lx\n", mem2);
#undef TEST_MEM_SIZE
#if IS_ENABLED(CONFIG_MMU)
#define TEST_MEM_SIZE 1024 * 1024 * 1
    mem2 = malloc(TEST_MEM_SIZE);
    printf("alloc mem 0x%lx\n", mem2);

    CuAssert(cu, "malloc failed\n", mem2 != NULL);
    memset(mem2, 0, TEST_MEM_SIZE);
    free(mem2);
    printf("free mem 0x%lx\n", mem2);
#undef TEST_MEM_SIZE
#endif
    int i;

    for (i = 0; i < 1000; i++)
    {
        mem[i] = malloc(MK_PAGE_SIZE);
        if (mem[i] == NULL)
        {
            break;
        }
        // memset(mem[i], 0, 4096);
        for (int j = 0; j < MK_PAGE_SIZE; j++)
        {
            mem[i][j] = j % 256;
        }
        // printf("alloc %d 0x%lx\n", i, mem[i]);
    }
    for (int m = 0; m < i; m++)
    {
        for (int j = 0; j < MK_PAGE_SIZE; j++)
        {
            CuAssert(cu, "mem failed.\n", mem[m][j] == j % 256);
        }
        free(mem[m]);
        // printf("free %d 0x%lx\n", i, mem[i]);
    }
}
static CuSuite suite;
CuSuite *malloc_test_suite(void)
{
    CuSuiteInit(&suite);

    SUITE_ADD_TEST(&suite, malloc_test);

    return &suite;
}
