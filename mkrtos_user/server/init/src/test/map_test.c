#include "u_log.h"
#include "u_prot.h"
#include "u_factory.h"
#include "u_thread.h"
#include "u_task.h"
#include "u_ipc.h"
#include "u_hd_man.h"
#include "u_arch.h"
#include "u_util.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <CuTest.h>
#include <pthread.h>

#define STACK_SIZE PAGE_SIZE

static pthread_t th1;
static pthread_t th2;

static __attribute__((aligned(PAGE_SIZE))) char msg_buf0[MSG_BUG_LEN];
static __attribute__((aligned(PAGE_SIZE))) char msg_buf1[MSG_BUG_LEN];
static __attribute__((aligned(PAGE_SIZE))) uint8_t stack0[STACK_SIZE * 2];
static __attribute__((aligned(PAGE_SIZE))) uint8_t stack1[STACK_SIZE * 2];

static void hard_sleep(void)
{

    for (volatile int i; i < 10000000; i++)
        ;
}
#define TEST_STR "I am th2.\n"
static void *thread_test_func(void *arg)
{
    char *buf;
    umword_t len;
    ipc_msg_t *ipc_msg;
    obj_handler_t log_hd = handler_alloc();
    thread_msg_buf_get(-1, (umword_t *)(&buf), NULL);
    ipc_msg = (ipc_msg_t *)buf;
    ipc_msg->map_buf[0] = vpage_create_raw3(0, 0, log_hd).raw;
    thread_ipc_wait(ipc_timeout_create2(0, 0), NULL, -1);
    printf("srv recv:%s", buf);
    assert(strcmp(TEST_STR, buf) == 0);
    ulog_write_str(log_hd, "map test success.\n");
    hard_sleep();
    printf("thread_test_func.\n");
    thread_ipc_reply(msg_tag_init4(0, ROUND_UP(strlen(buf), WORD_BYTES), 0, 0), ipc_timeout_create2(0, 0));
    handler_free(log_hd);
    return NULL;
}
static void *thread_test_func2(void *arg)
{
    char *buf;
    umword_t len;
    ipc_msg_t *ipc_msg;

    thread_msg_buf_get(-1, (umword_t *)(&buf), NULL);
    ipc_msg = (ipc_msg_t *)buf;
    strcpy((char *)(ipc_msg->msg_buf), TEST_STR);
    ipc_msg->map_buf[0] = vpage_create_raw3(KOBJ_DELETE_RIGHT, VPAGE_FLAGS_MAP, LOG_PROT).raw;
    thread_ipc_call(msg_tag_init4(0, ROUND_UP(strlen((char *)(ipc_msg->msg_buf)), WORD_BYTES), 1, 0),
                    pthread_hd_get(th1), ipc_timeout_create2(0, 0));
    printf("th2:%s", buf);
    assert(strcmp(TEST_STR, buf) == 0);
    printf("thread_test_func2.\n");
    return NULL;
}
/**
 * @brief 启动两个线程并进行ipc测试
 *
 */
static void map_test(CuTest *test)
{
    int ret;

    ret = pthread_create(&th1, NULL, thread_test_func, NULL);
    CuAssert(test, "pthread create error.\n", ret == 0);
    pthread_create(&th2, NULL, thread_test_func2, NULL);
    CuAssert(test, "pthread create error.\n", ret == 0);

    CuAssert(test, "pthread_join fail.\n", pthread_join(th1, NULL) == 0);
    CuAssert(test, "pthread_join fail.\n", pthread_join(th2, NULL) == 0);
}
static CuSuite suite;
CuSuite *map_test_suite(void)
{
    CuSuiteInit(&suite);

    SUITE_ADD_TEST(&suite, map_test);

    return &suite;
}
