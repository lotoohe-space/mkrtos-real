#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <CuTest.h>
#include <u_factory.h>
#include <u_hd_man.h>
#include <u_vmam.h>
#include <stdio.h>
static void vmm_large_block_test(CuTest *cu)
{
    addr_t addr;
    msg_tag_t tag;
#define TEST_CN 100
#define TEST_MM_SIZE (1024 * 1024 * 1)
    for (int i = 0; i < TEST_CN; i++)
    {
        tag = u_vmam_alloc(VMA_PROT, vma_addr_create(VPAGE_PROT_RWX, 0, 0), TEST_MM_SIZE, 0, &addr);
        CuAssert(cu, "vmam alloc faile.\n", msg_tag_get_val(tag) >= 0);
        memset((void *)addr, 0, TEST_MM_SIZE);
        u_vmam_free(VMA_PROT, addr, TEST_MM_SIZE);
    }
#undef TEST_MM_SIZE
#undef TEST_CN
}
static void vmm_small_block_test(CuTest *cu)
{
    addr_t addr;
    msg_tag_t tag;
#define TEST_CN 100
#define TEST_MM_SIZE (PAGE_SIZE)
    for (int i = 0; i < TEST_CN; i++)
    {
        tag = u_vmam_alloc(VMA_PROT, vma_addr_create(VPAGE_PROT_RWX, 0, 0), TEST_MM_SIZE, 0, &addr);
        CuAssert(cu, "vmam alloc faile.\n", msg_tag_get_val(tag) >= 0);
        memset((void *)addr, 0, TEST_MM_SIZE);
        u_vmam_free(VMA_PROT, addr, TEST_MM_SIZE);
    }
#undef TEST_MM_SIZE
#undef TEST_CN
}
#define TEST_MEM_CN 100
static void *test_main[TEST_MEM_CN];
static void vmm_press_block_test(CuTest *cu)
{
    addr_t addr;
    msg_tag_t tag;
#define TEST_MM_SIZE (PAGE_SIZE)
    for (int i = 0; i < TEST_MEM_CN; i++)
    {
        tag = u_vmam_alloc(VMA_PROT, vma_addr_create(VPAGE_PROT_RWX, 0, 0), TEST_MM_SIZE, 0, &addr);
        CuAssert(cu, "vmam alloc faile.\n", msg_tag_get_val(tag) >= 0);
        memset((void *)addr, 0x55, TEST_MM_SIZE);
        // printf("alloc umem:0x%lx\n", addr);
        test_main[i] = (void *)addr;
    }
    for (int i = 0; i < TEST_MEM_CN; i++)
    {
        for (int j = 0; j < TEST_MM_SIZE; j++)
        {
            uint8_t *tmp_mm = test_main[i];
            if (tmp_mm[j] != 0x55)
            {
                printf("mem test fail. i:%d j:%d\n", i, j);
            }
            assert(tmp_mm[j] == 0x55);
        }
        u_vmam_free(VMA_PROT, (addr_t)(test_main[i]), TEST_MM_SIZE);
    }

#undef TEST_MM_SIZE
#undef TEST_MEM_CN
}
CuSuite *vmm_test_suite(void)
{
    CuSuite *suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, vmm_large_block_test);
    SUITE_ADD_TEST(suite, vmm_small_block_test);
    SUITE_ADD_TEST(suite, vmm_press_block_test);

    return suite;
}