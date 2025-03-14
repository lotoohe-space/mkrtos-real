#include "u_log.h"
#include "u_prot.h"

#include "u_factory.h"
#include "u_thread.h"
#include "u_task.h"
#include "u_ipc.h"
#include "u_hd_man.h"
#include <assert.h>
#include <stdio.h>
#include "test.h"
#include "u_sleep.h"
#include <pthread.h>
#include <CuTest.h>
#define TEST_THREAD_NUM 50

static pthread_t th_array[TEST_THREAD_NUM];

static void *thread_test_func(void *arg)
{
    printf(".\n");
    return NULL;
}

static void thread_press_test(CuTest *cu)
{
    for (int i = 0; i < TEST_THREAD_NUM; i++)
    {
        CuAssert(cu, "pthread create error \n",
                 pthread_create(&th_array[i], NULL, thread_test_func, NULL) == 0);
    }
    for (int i = 0; i < TEST_THREAD_NUM; i++)
    {
        CuAssert(cu, "pthread join error\n",
        pthread_join(th_array[i], NULL) == 0);
    }
}
static CuSuite suite;
CuSuite *pthread_press_test_suite(void)
{
    CuSuiteInit(&suite);

    SUITE_ADD_TEST(&suite, thread_press_test);

    return &suite;
}
