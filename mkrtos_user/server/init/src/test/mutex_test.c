
#include "u_factory.h"
#include "u_hd_man.h"
#include "u_task.h"
#include "u_thread.h"
#include <CuTest.h>
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <u_mutex.h>
#include <u_sleep.h>
#include <unistd.h>

static pthread_t pth1;
static pthread_t pth2;
static pthread_t pth3;
static u_mutex_t mutex_lock;
static int test_cn;
static __attribute__((optimize(0)))  void hard_sleep(void)
{
    volatile int i = 999999999;
    while(i--);
}
static void *thread_th1(void *arg)
{
    thread_run(-1, 5);
    u_mutex_lock(&mutex_lock, 0, NULL);
    hard_sleep();
    u_mutex_unlock(&mutex_lock);
    return NULL;
}
static void *thread_th2(void *arg)
{
    u_sleep_ms(10);
    thread_run(-1, 6);
    u_sleep_ms(20);
    while(test_cn < 10000000) {
        test_cn++;
    }
    return NULL;
}
static void *thread_th3(void *arg)
{
    u_sleep_ms(20);
    thread_run(-1, 7);
    u_mutex_lock(&mutex_lock, 0, NULL);
    hard_sleep();
    if (test_cn != 0) {
        u_mutex_unlock(&mutex_lock);
        return (void *)-1;
    }
    u_mutex_unlock(&mutex_lock);
    return NULL;
}
/**
 * 测试优先级继承是否生效
 */
static void u_mutex_test(CuTest *tc)
{
    obj_handler_t mutex_hd;
    int ret;

    mutex_hd = handler_alloc();
    CuAssert(tc, "hd alloc fail.\n", mutex_hd != HANDLER_INVALID);
    ret = u_mutex_init(&mutex_lock, mutex_hd);
    CuAssert(tc, "u_mutex_init fail.\n", ret >= 0);

    CuAssert(tc, "pthread_create fail.\n", pthread_create(&pth1, NULL, thread_th1, NULL) == 0);
    CuAssert(tc, "pthread_create fail.\n", pthread_create(&pth2, NULL, thread_th2, NULL) == 0);
    CuAssert(tc, "pthread_create fail.\n", pthread_create(&pth3, NULL, thread_th3, NULL) == 0);
    if (pth1 != PTHREAD_NULL) {
        CuAssert(tc, "pthread_join fail.\n", pthread_join(pth1, NULL) == 0);
    }
    if (pth2 != PTHREAD_NULL) {
        CuAssert(tc, "pthread_join fail.\n", pthread_join(pth2, NULL) == 0);
    }
    if (pth3 != PTHREAD_NULL) {
        umword_t arg;
        CuAssert(tc, "pthread_join fail.\n", pthread_join(pth3, (void **)&arg) == 0);
        CuAssert(tc, "pthread_join fail.\n", arg == 0);
    }
}
static CuSuite suite;
CuSuite *mutex_test_suite(void)
{
    CuSuiteInit(&suite);

    SUITE_ADD_TEST(&suite, u_mutex_test);

    return &suite;
}
