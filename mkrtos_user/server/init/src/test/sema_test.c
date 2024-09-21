
#include "u_factory.h"
#include "u_sema.h"
#include "u_hd_man.h"
#include "u_task.h"
#include <u_sleep.h>
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <CuTest.h>
#include <unistd.h>

int u_sema_test(void)
{
    msg_tag_t tag;
    obj_handler_t sema_hd;

    sema_hd = handler_alloc();
    assert(sema_hd != HANDLER_INVALID);
    tag = facotry_create_sema(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, sema_hd), 0, 1);
    assert(msg_tag_get_val(tag) >= 0);

    u_sema_up(sema_hd);
    u_sema_down(sema_hd);

    handler_free_umap(sema_hd);
    return 0;
}
static pthread_t pth1;
static pthread_t pth2;
static pthread_t pth3;
static obj_handler_t sema_hd2;
#define TEST_CN 10
static void *thread_th1(void *arg)
{
    int j = 0;
    while (1)
    {
        printf("sema_up start\n");
        u_sema_up(sema_hd2);
        u_sleep_ms(100);
        printf("sema_up end\n");
        if (j == TEST_CN * 2 + 2)
        {
            break;
        }
        j++;
    }
    printf("%s sema up exit.\n", __func__);
    return NULL;
}
static void *thread_th2(void *arg)
{
    int j = 0;
    while (1)
    {
        printf("sema_down start\n");
        u_sema_down(sema_hd2);
        u_sleep_ms(50);
        printf("sema_down end\n");
        if (j == TEST_CN)
        {
            break;
        }
        j++;
    }
    printf("%s sema down exit.\n", __func__);
    return NULL;
}
static void *thread_th3(void *arg)
{
    int j = 0;
    while (1)
    {
        printf("sema_down2 start\n");
        u_sema_down(sema_hd2);
        u_sleep_ms(50);
        printf("sema_down2 end\n");
        if (j == TEST_CN)
        {
            break;
        }
        j++;
    }
    printf("%s sema down exit.\n", __func__);
    return NULL;
}
static void u_sema_test2(CuTest *tc)
{
    msg_tag_t tag;

    sema_hd2 = handler_alloc();
    CuAssert(tc, "hd alloc fail.\n", sema_hd2 != HANDLER_INVALID);
    tag = facotry_create_sema(FACTORY_PROT,
                              vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, sema_hd2), 0, 1);
    CuAssert(tc, "hd alloc fail.\n", msg_tag_get_val(tag) >= 0);
    CuAssert(tc, "pthread_create fail.\n", pthread_create(&pth1, NULL, thread_th1, NULL) == 0);
    CuAssert(tc, "pthread_create fail.\n", pthread_create(&pth2, NULL, thread_th2, NULL) == 0);
    CuAssert(tc, "pthread_create fail.\n", pthread_create(&pth3, NULL, thread_th3, NULL) == 0);
    if (pth1 != PTHREAD_NULL)
    {
        CuAssert(tc, "pthread_join fail.\n", pthread_join(pth1, NULL) == 0);
    }
    if (pth2 != PTHREAD_NULL)
    {
        CuAssert(tc, "pthread_join fail.\n", pthread_join(pth2, NULL) == 0);
    }
    if (pth3 != PTHREAD_NULL)
    {
        CuAssert(tc, "pthread_join fail.\n", pthread_join(pth3, NULL) == 0);
    }
}
static CuSuite suite;
CuSuite *sema_test_suite(void)
{
    CuSuiteInit(&suite);

    SUITE_ADD_TEST(&suite, u_sema_test2);

    return &suite;
}
