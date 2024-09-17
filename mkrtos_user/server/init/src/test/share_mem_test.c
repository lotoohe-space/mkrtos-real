#include "u_types.h"
#include "u_prot.h"
#include "u_factory.h"
#include "u_task.h"
#include "u_hd_man.h"
#include "u_share_mem.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <CuTest.h>
static void sharea_mem_test(CuTest *cu)
{
    addr_t addr;
    umword_t size;
    obj_handler_t hd = handler_alloc();
    assert(hd != HANDLER_INVALID);
    msg_tag_t tag = facotry_create_share_mem(FACTORY_PROT,
                                             vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, hd),
                                             SHARE_MEM_CNT_BUDDY_CNT,
                                             PAGE_SIZE * 100);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = share_mem_map(hd, vma_addr_create(VPAGE_PROT_RW, VMA_ADDR_RESV, 0), &addr, &size);
    assert(msg_tag_get_prot(tag) >= 0);
    memset((void *)addr, 0, size);
    share_mem_unmap(hd);
    tag = share_mem_map(hd, vma_addr_create(VPAGE_PROT_RW, VMA_ADDR_RESV, 0), &addr, &size);
    assert(msg_tag_get_prot(tag) >= 0);
    memset((void *)addr, 0, size);
    share_mem_unmap(hd);
    handler_free_umap(hd);

    hd = handler_alloc();
    assert(hd != HANDLER_INVALID);
    tag = facotry_create_share_mem(FACTORY_PROT,
                                   vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, hd),
                                   SHARE_MEM_CNT_DPD,
                                   PAGE_SIZE * 100);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = share_mem_map(hd, vma_addr_create(VPAGE_PROT_RW, VMA_ADDR_RESV, 0), &addr, &size);
    assert(msg_tag_get_prot(tag) >= 0);
    memset((void *)addr, 0, size);
    share_mem_unmap(hd);
    tag = share_mem_map(hd, vma_addr_create(VPAGE_PROT_RW, VMA_ADDR_RESV, 0), &addr, &size);
    assert(msg_tag_get_prot(tag) >= 0);
    memset((void *)addr, 0, size);
    share_mem_unmap(hd);
    handler_free_umap(hd);
}
static CuSuite suite;
CuSuite *sharem_mem_test_suite(void)
{
    CuSuiteInit(&suite);

    SUITE_ADD_TEST(&suite, sharea_mem_test);

    return &suite;
}
