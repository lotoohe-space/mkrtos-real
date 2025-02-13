#if 0
#include "u_log.h"
#include "u_prot.h"
#include "u_factory.h"
#include "u_thread.h"
#include "u_task.h"
#include "u_ipc.h"
#include "u_hd_man.h"
#include "u_task.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <u_sleep.h>
#include <u_thread_util.h>
#include <CuTest.h>
#define DEBUG_IPC_CALL 1

static umword_t th1_hd = 0;
static umword_t th2_hd = 0;
static umword_t th3_hd = 0;
static umword_t th4_hd = 0;

static __attribute__((aligned(4096))) char msg_buf0[4096];
static __attribute__((aligned(4096))) char msg_buf1[4096];
static __attribute__((aligned(4096))) char msg_buf2[4096];
static __attribute__((aligned(4096))) char msg_buf3[4096];
#define STACK_SIZE 2048
static __attribute__((aligned(8))) uint8_t stack0[STACK_SIZE];
static __attribute__((aligned(8))) uint8_t stack1[STACK_SIZE];
static __attribute__((aligned(8))) uint8_t stack2[STACK_SIZE];
static __attribute__((aligned(8))) uint8_t stack3[STACK_SIZE];

static void hard_sleep(void)
{

    for (volatile int i; i < 10000000; i++)
        ;
}
static int test_cn = 0;
static void thread_test_func(void)
{
    char *buf;
    umword_t len;

    printf("%s:%d thread 0 init.\n", __func__, __LINE__);
    thread_msg_buf_get(th1_hd, (umword_t *)(&buf), NULL);
    while (1)
    {
        thread_ipc_wait(ipc_timeout_create2(0, 0), NULL, -1);
        // printf("srv recv:%s", buf);
        // hard_sleep();
        // u_sleep_ms(10);
        // buf[0] = ',';
        test_cn++;
        thread_ipc_reply(msg_tag_init4(0, ROUND_UP(strlen(buf), WORD_BYTES), 0, 0), ipc_timeout_create2(0, 0));
        if (test_cn >= 1000)
        {
            break;
        }
    }
    // printf("thread_test_func.\n");
    while (1)
    {
        u_sleep_ms(100000);
    }
    task_unmap(TASK_PROT, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, th1_hd));
    printf("Error\n");
}
static void thread_test_func2(void)
{
    char *buf;
    umword_t len;

    printf("%s:%d thread 1 init.\n", __func__, __LINE__);
    thread_msg_buf_get(th2_hd, (umword_t *)(&buf), NULL);
    while (1)
    {
        strcpy(buf, "I am th2.\n");
        thread_ipc_call(msg_tag_init4(0, ROUND_UP(strlen(buf), WORD_BYTES), 0, 0), th1_hd, ipc_timeout_create2(0, 0));
        printf("th2:%s", buf);
        if (test_cn >= 1000)
        {
            break;
        }
    }
    printf("thread_test_func2.\n");
    while (1)
    {
        u_sleep_ms(100000);
    }
    task_unmap(TASK_PROT, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, th2_hd));
    printf("Error\n");
}

static void thread_test_func3(void)
{
    char *buf;
    umword_t len;

    printf("%s:%d thread 2 init.\n", __func__, __LINE__);
    thread_msg_buf_get(th3_hd, (umword_t *)(&buf), &len);
    memset(buf, 0, len);
    while (1)
    {
        strcpy(buf, "I am th3.\n");
        thread_ipc_call(msg_tag_init4(0, ROUND_UP(strlen(buf), WORD_BYTES), 0, 0), th1_hd, ipc_timeout_create2(0, 0));
        printf("th3:%s", buf);
        if (test_cn >= 1000)
        {
            break;
        }
    }
    printf("thread_test_func3.\n");
    while (1)
    {
        u_sleep_ms(100000);
    }
    task_unmap(TASK_PROT, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, th3_hd));
    printf("Error\n");
}
static void thread_test_func4(void)
{
    char *buf;
    umword_t len;

    printf("%s:%d thread 3 init.\n", __func__, __LINE__);
    thread_msg_buf_get(th4_hd, (umword_t *)(&buf), &len);
    memset(buf, 0, len);
    while (1)
    {
        strcpy(buf, "I am th4.\n");
        thread_ipc_call(msg_tag_init4(0, ROUND_UP(strlen(buf), WORD_BYTES), 0, 0), th1_hd, ipc_timeout_create2(0, 0));
        printf("th4:%s", buf);
        if (test_cn >= 1000)
        {
            break;
        }
    }
    printf("thread_test_func4.\n");
    while (1)
    {
        u_sleep_ms(100000);
    }
    task_unmap(TASK_PROT, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, th3_hd));
    printf("Error\n");
}
/**
 * @brief 启动两个线程并进行ipc测试
 *
 */
static void ipc_test(CuTest *cu)
{
    msg_tag_t tag;
    th1_hd = handler_alloc();
    assert(th1_hd != HANDLER_INVALID);
    th2_hd = handler_alloc();
    assert(th2_hd != HANDLER_INVALID);
    th3_hd = handler_alloc();
    assert(th3_hd != HANDLER_INVALID);

    memset(msg_buf0, 0, sizeof(msg_buf0));
    memset(msg_buf1, 0, sizeof(msg_buf1));
    memset(msg_buf2, 0, sizeof(msg_buf2));
    memset(msg_buf3, 0, sizeof(msg_buf3));

    tag = factory_create_thread(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, th1_hd));
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_bind_task(th1_hd, TASK_THIS);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_msg_buf_set(th1_hd, msg_buf0);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_exec_regs(th1_hd, (umword_t)thread_test_func, (umword_t)stack0 + STACK_SIZE, TASK_RAM_BASE(), 0);
    task_set_obj_name(TASK_THIS, th1_hd, "svr_th");
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_run_cpu(th1_hd, 2, 0);
    assert(msg_tag_get_prot(tag) >= 0);

    assert(msg_tag_get_prot(tag) >= 0);
    tag = factory_create_thread(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, th2_hd));
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_bind_task(th2_hd, TASK_THIS);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_msg_buf_set(th2_hd, msg_buf1);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_exec_regs(th2_hd, (umword_t)thread_test_func2, (umword_t)stack1 + STACK_SIZE, TASK_RAM_BASE(), 0);
    assert(msg_tag_get_prot(tag) >= 0);
    task_set_obj_name(TASK_THIS, th2_hd, "cli0_th");
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_run_cpu(th2_hd, 2, 0);

    tag = factory_create_thread(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, th3_hd));
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_bind_task(th3_hd, TASK_THIS);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_msg_buf_set(th3_hd, msg_buf2);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_exec_regs(th3_hd, (umword_t)thread_test_func3, (umword_t)stack2 + STACK_SIZE, TASK_RAM_BASE(), 0);
    assert(msg_tag_get_prot(tag) >= 0);
    task_set_obj_name(TASK_THIS, th3_hd, "cli1_th");
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_run_cpu(th3_hd, 2, 0);

    tag = factory_create_thread(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, th4_hd));
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_bind_task(th4_hd, TASK_THIS);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_msg_buf_set(th4_hd, msg_buf3);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_exec_regs(th4_hd, (umword_t)thread_test_func4, (umword_t)stack3 + STACK_SIZE, TASK_RAM_BASE(), 0);
    assert(msg_tag_get_prot(tag) >= 0);
    task_set_obj_name(TASK_THIS, th4_hd, "cli2_th");
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_run_cpu(th4_hd, 2, 0);

    while (test_cn < 1000)
    {
        u_sleep_ms(10);
    }
    u_thread_del(th1_hd);
    u_thread_del(th2_hd);
    u_thread_del(th3_hd);
    u_thread_del(th4_hd);
    printf("test_cn:%d.\n", test_cn);
}
static CuSuite suite;
CuSuite *ipc_test_suite(void)
{
    CuSuiteInit(&suite);
    SUITE_ADD_TEST(&suite, ipc_test);

    return &suite;
}
#endif