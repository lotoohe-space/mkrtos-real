
#include "config.h"
#include "knl_service.h"
#include "mm.h"

#define TEST_MEM_NUM 10
int test_mem(void)
{
    size_t m_free_size = free_size();
    kprint("=======start test mem\n");

    kprint("sizeof(mem_heap_t) is %d\n", sizeof(mem_heap_t));

    void *mem_array[TEST_MEM_NUM] = {
        NULL
    };
    knl_mem_trace();
    for (int i = 0; i < TEST_MEM_NUM; i++) {
        mem_array[i] = malloc(i*3);
    }
    kprint("====================\n");
    knl_mem_trace();
    for (int i = 0; i< TEST_MEM_NUM; i++) {
        free(mem_array[i]);
    }
    kprint("====================\n");
    knl_mem_trace();
    MKRTOS_ASSERT(m_free_size == free_size());

    void *mem_tmp;

    for (int i = 0; i < 8; i++) {
        mem_tmp = malloc_align(1234, 1 << i);
        MKRTOS_ASSERT(mem_tmp);
        free_align(mem_tmp);
        kprint("====================\n");
        knl_mem_trace();
    }
    MKRTOS_ASSERT(m_free_size == free_size());

// void *user_malloc(uint32_t size,const char* name);
// void user_free(void *mem);
// void knl_mem_trace(void);
// struct mem_heap *knl_mem_get_free(struct mem_heap *next,
// 	int32_t hope_size, uint32_t *ret_addr);
    kprint("=======end test mem\n");
    return 0;
}

INIT_REG(test_mem, INIT_TEST_LVL);
