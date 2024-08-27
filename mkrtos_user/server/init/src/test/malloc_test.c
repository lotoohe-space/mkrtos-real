#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <CuTest.h>
#include <stdio.h>
#include <u_types.h>
static uint8_t *mem[1000];
static void malloc_test(CuTest *cu)
{
#define TEST_MEM_SIZE 1024 * 1024 * 1
    void *mem2 = malloc(TEST_MEM_SIZE);
    printf("alloc mem 0x%lx\n", mem2);

    CuAssert(cu, "malloc failed\n", mem2 != NULL);
    memset(mem2, 0, TEST_MEM_SIZE);
    free(mem2);
    printf("free mem 0x%lx\n", mem2);
#undef TEST_MEM_SIZE

    for (int i = 0; i < 1000; i++)
    {
        mem[i] = malloc(4096);
        CuAssert(cu, "malloc failed\n", mem[i] != NULL);
        // memset(mem[i], 0, 4096);
        for (int j = 0; j < 4096; j++)
        {
            mem[i][j] = j % 256;
        }
        // printf("alloc %d 0x%lx\n", i, mem[i]);
    }
    for (int i = 0; i < 1000; i++)
    {
        for (int j = 0; j < 4096; j++)
        {
            CuAssert(cu, "mem failed.\n", mem[i][j] == j % 256);
        }
        free(mem[i]);
        // printf("free %d 0x%lx\n", i, mem[i]);
    }
}
CuSuite *malloc_test_suite(void)
{
    CuSuite *suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, malloc_test);

    return suite;
}
