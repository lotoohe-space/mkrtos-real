

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
#include <u_thread_util.h>
#define TEST_THREAD_NUM 50
#define STACK_SIZE      (4096 * 2)
static obj_handler_t th_hd;
static __attribute__((aligned(PAGE_SIZE))) uint8_t stack0[STACK_SIZE];

#define HAND_SLEEP_DELAY 1000000000
static void thread_hard_sleep(volatile int i)
{
    volatile int j;

    for (j = 0; j < i; j++)
        ;
}

static void thread_test_func(void)
{
    while (1) {
        u_sleep_ms(100000000);
    }
}
static void thread_test_func_hard_sleep(void)
{
    while (1) {
        thread_hard_sleep(HAND_SLEEP_DELAY);
    }
}

static void thread_base_test(CuTest *cu)
{
    int ret;

    for (int i = 0; i < CONFIG_CPU; i++) {
        ret = u_thread_create(&th_hd, stack0 + STACK_SIZE, NULL, thread_test_func);
        CuAssert(cu, "u_thread_create failed.\n", ret >= 0);
        u_thread_run_cpu(th_hd, 2, i);
        u_sleep_ms(100);
        u_thread_del(th_hd);
    }

    for (int i = 0; i < CONFIG_CPU; i++) {
        ret = u_thread_create(&th_hd, stack0 + STACK_SIZE, NULL, thread_test_func);
        CuAssert(cu, "u_thread_create failed.\n", ret >= 0);
        u_thread_run_cpu(th_hd, 2, i);
        thread_hard_sleep(HAND_SLEEP_DELAY/3);
        u_thread_del(th_hd);
    }
}
static CuSuite suite;
CuSuite *thread_base_test_suite(void)
{
    CuSuiteInit(&suite);

    SUITE_ADD_TEST(&suite, thread_base_test);

    return &suite;
}
