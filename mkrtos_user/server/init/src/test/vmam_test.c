#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <CuTest.h>
#include <u_factory.h>
#include <u_hd_man.h>
#include <u_vmam.h>
#include <stdio.h>

#if IS_ENABLED(CONFIG_MMU)
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
#endif
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
    int i;
    int m;
    addr_t addr;
    msg_tag_t tag;
#define TEST_MM_SIZE (PAGE_SIZE)
    for (i = 0; i < TEST_MEM_CN; i++)
    {
        tag = u_vmam_alloc(VMA_PROT, vma_addr_create(VPAGE_PROT_RWX, 0, 0), TEST_MM_SIZE, 0, &addr);
        if (msg_tag_get_val(tag) < 0)
        {
            break;
        }
        memset((void *)addr, 0x55, TEST_MM_SIZE);
        // printf("alloc umem:0x%lx\n", addr);
        test_main[i] = (void *)addr;
    }
    for (m = 0; m < i; m++)
    {
        for (int j = 0; j < TEST_MM_SIZE; j++)
        {
            uint8_t *tmp_mm = test_main[m];
            if (tmp_mm[j] != 0x55)
            {
                printf("mem test fail. i:%d j:%d\n", m, j);
            }
            assert(tmp_mm[j] == 0x55);
        }
        u_vmam_free(VMA_PROT, (addr_t)(test_main[m]), TEST_MM_SIZE);
    }

#undef TEST_MM_SIZE
#undef TEST_MEM_CN
}
#define TEST_MEM_CN 30
static void vmm_press_block_sim_page_fault_test(CuTest *cu)
{
    int i;
    int m;
    addr_t addr;
    msg_tag_t tag;

    memset(test_main, 0, sizeof(test_main));

    tag = u_vmam_alloc(VMA_PROT, vma_addr_create(VPAGE_PROT_RWX, VMA_ADDR_PAGE_FAULT_SIM, 0), 1234, 0, &addr);
    assert(msg_tag_get_val(tag) < 0);
#define TEST_MM_SIZE (1024)
    for (i = 0; i < TEST_MEM_CN; i++)
    {
        tag = u_vmam_alloc(VMA_PROT, vma_addr_create(VPAGE_PROT_RWX, VMA_ADDR_PAGE_FAULT_SIM, 0), TEST_MM_SIZE, 0, &addr);
        if (msg_tag_get_val(tag) < 0)
        {
            break;
        }
        memset((void *)addr, 0x55, TEST_MM_SIZE);
        // printf("alloc umem:0x%lx\n", addr);
        test_main[i] = (void *)addr;
    }
    uint8_t *mem0 = test_main[0];
    uint8_t *mem1 = test_main[1];
    for (int i = 0; i < 100; i++)
    {
        *mem0 = *mem1;
        *mem1 = *mem0;
    }
    for (m = 0; m < i; m++)
    {
        for (int j = 0; j < TEST_MM_SIZE; j++)
        {
            uint8_t *tmp_mm = test_main[m];
            if (tmp_mm[j] != 0x55)
            {
                printf("mem test fail. i:%d j:%d\n", m, j);
            }
            assert(tmp_mm[j] == 0x55);
        }
        u_vmam_free(VMA_PROT, (addr_t)(test_main[m]), TEST_MM_SIZE);
    }

#undef TEST_MM_SIZE
#undef TEST_MEM_CN
}
#include <u_thread_util.h>
#include <u_sleep.h>
#include <u_log.h>
static obj_handler_t th_hd;
static void mem_sim_thread(void)
{
    int i = 0;
    uint8_t data[512];
    while (1)
    {
        memset(data, 0, 512);
        ulog_write_str(LOG_PROT, "i am live.\n");
        u_sleep_ms(1000);
        i++;
        if (i >= 40)
        {
            break;
        }
    }
    while (1)
    {
        u_sleep_ms(100000);
    }
}
static void vmm_press_block_sim_thread_test(CuTest *cu)
{
    msg_tag_t tag;
    addr_t addr;
    int ret;
    tag = u_vmam_alloc(VMA_PROT, vma_addr_create(VPAGE_PROT_RWX, VMA_ADDR_PAGE_FAULT_SIM, 0), PAGE_SIZE * 4, 0, &addr);
    CuAssert(cu, "vmam alloc failed.\n", msg_tag_get_val(tag) >= 0);
    addr_t msg_addr;
    tag = u_vmam_alloc(VMA_PROT, vma_addr_create(VPAGE_PROT_RWX, VMA_ADDR_PAGE_FAULT_SIM, 0), PAGE_SIZE, 0, &msg_addr);
    CuAssert(cu, "vmam alloc failed.\n", msg_tag_get_val(tag) >= 0);
    memset((void *)addr, 0, PAGE_SIZE * 4);
    memset((void *)msg_addr, 0, PAGE_SIZE);
    printf("sim_addr:0x%lx, msg_addr:0x%lx\n", addr, msg_addr);
    ret = u_thread_create(&th_hd, (void *)(addr + (PAGE_SIZE * 4) - sizeof(void *) * 2), (void *)msg_addr, mem_sim_thread);
    CuAssert(cu, "u_thread_create failed.\n", ret >= 0);
    u_thread_run(th_hd, 2);
    while (1)
    {
        u_sleep_ms(10000);
    }
}

static CuSuite suite;
CuSuite *vmm_test_suite(void)
{
    CuSuiteInit(&suite);

#if IS_ENABLED(CONFIG_MMU)
    SUITE_ADD_TEST(&suite, vmm_large_block_test);
#endif
#if IS_ENABLED(CONFIG_MPU)
    SUITE_ADD_TEST(&suite, vmm_small_block_test);
    SUITE_ADD_TEST(&suite, vmm_press_block_test);
#endif
    SUITE_ADD_TEST(&suite, vmm_press_block_sim_page_fault_test);
    // mpu模拟缺页的内存不能用作栈内存，这是硬件限制导致的！
    // SUITE_ADD_TEST(&suite, vmm_press_block_sim_thread_test);

    return &suite;
}
