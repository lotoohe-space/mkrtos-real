#include "u_types.h"
#include <stdio.h>
#include <pthread.h>
#include "u_log.h"
#include "u_prot.h"
#include "u_mm.h"
#include "u_factory.h"
#include "u_thread.h"
#include "u_task.h"
#include "u_ipc.h"
#include "u_hd_man.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <CuTest.h>
#define TEST_CNT 10
static pthread_mutex_t lock;
static pthread_t pth;
static pthread_t pth2;

#define STACK_SIZE PAGE_SIZE
static __attribute__((aligned(PAGE_SIZE))) uint8_t stack0[STACK_SIZE * 2];
static __attribute__((aligned(PAGE_SIZE))) uint8_t stack1[STACK_SIZE * 2];

static void hard_sleep(void)
{
    for (volatile int i; i < 1000000000; i++)
        ;
}
static void *thread_test_func(void *arg)
{
    usleep(50000);

    int i = TEST_CNT;
    while (i--)
    {
        pthread_mutex_lock(&lock);
        printf("thread 1 ..\n");
        usleep(50000);
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}
static void *thread_test_func2(void *arg)
{

    int i = TEST_CNT;
    while (i--)
    {
        pthread_mutex_lock(&lock);
        printf("thread 2 ..\n");
        usleep(50000);
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

/**
 *
 */
static void pthread_lock_test(CuTest *cu)
{
    pthread_attr_t attr;

    pthread_mutex_init(&lock, NULL);
    pthread_attr_init(&attr);
    pthread_attr_setstack(&attr, stack0, STACK_SIZE);
    CuAssert(cu, "pthread create error.\n", pthread_create(&pth, &attr, thread_test_func, NULL) == 0);
    pthread_attr_setstack(&attr, stack1, STACK_SIZE);
    CuAssert(cu, "pthread create error.\n", pthread_create(&pth2, &attr, thread_test_func2, NULL) == 0);
    pthread_join(pth, NULL);
    pthread_join(pth2, NULL);
    printf("%s:%d test ok.\n", __func__, __LINE__);
}
static CuSuite suite;
CuSuite *pthread_lock_test_suite(void)
{
    CuSuiteInit(&suite);

    SUITE_ADD_TEST(&suite, pthread_lock_test);

    return &suite;
}
