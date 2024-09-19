#include "test.h"
#include "u_factory.h"
#include "u_hd_man.h"
#include "u_ipc.h"
#include "u_log.h"
#include "u_prot.h"
#include "u_sleep.h"
#include "u_task.h"
#include "u_thread.h"
#include <CuTest.h>
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#define TEST_THREAD_NUM 50

static pthread_t th_test;

static void *thread_test_func(void *arg)
{
    printf(".\n");
    return NULL;
}

static void thread_base_test(CuTest *cu)
{
    CuAssert(cu, "pthread create error \n",
             pthread_create(&th_test, NULL, thread_test_func, NULL) == 0);
    CuAssert(cu, "pthread join error\n",
             pthread_join(th_test, NULL) == 0);
}
static CuSuite suite;
CuSuite *pthread_base_test_suite(void)
{
    CuSuiteInit(&suite);

    SUITE_ADD_TEST(&suite, thread_base_test);

    return &suite;
}
