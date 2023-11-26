#include "u_log.h"
#include "u_prot.h"
#include "u_mm.h"
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
#define DEBUG_IPC_CALL 1

static umword_t th1_hd = 0;
static umword_t th2_hd = 0;

static char msg_buf0[MSG_BUG_LEN];
static char msg_buf1[MSG_BUG_LEN];
#define STACK_SIZE 1024
static __attribute__((aligned(8))) uint8_t stack0[STACK_SIZE];
static __attribute__((aligned(8))) uint8_t stack1[STACK_SIZE];

static void hard_sleep(void)
{

    for (volatile int i; i < 10000000; i++)
        ;
}
static void thread_test_func(void)
{
    char *buf;
    umword_t len;
    ipc_msg_t *ipc_msg;
    obj_handler_t log_hd = handler_alloc();
    thread_msg_buf_get(th1_hd, (umword_t *)(&buf), NULL);
    ipc_msg = (ipc_msg_t *)buf;
    ipc_msg->map_buf[0] = vpage_create_raw3(0, 0, log_hd).raw;
    thread_ipc_wait(ipc_timeout_create2(0, 0), NULL);
    printf("srv recv:%s", buf);
    ulog_write_str(log_hd, "map test success.\n");
    hard_sleep();
    thread_ipc_reply(msg_tag_init4(0, ROUND_UP(strlen(buf), WORD_BYTES), 0, 0), ipc_timeout_create2(0, 0));
    printf("thread_test_func.\n");
    handler_free(log_hd);
    task_unmap(TASK_PROT, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, th1_hd));
    printf("Error\n");
}
static void thread_test_func2(void)
{
    char *buf;
    umword_t len;
    ipc_msg_t *ipc_msg;

    thread_msg_buf_get(th2_hd, (umword_t *)(&buf), NULL);
    ipc_msg = (ipc_msg_t *)buf;
    strcpy((char *)(ipc_msg->msg_buf), "I am th2.\n");
    ipc_msg->map_buf[0] = vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, LOG_PROT).raw;
    thread_ipc_call(msg_tag_init4(0, ROUND_UP(strlen((char *)(ipc_msg->msg_buf)), WORD_BYTES), 1, 0), th1_hd, ipc_timeout_create2(0, 0));
    printf("th2:%s", buf);
    printf("thread_test_func2.\n");
    task_unmap(TASK_PROT, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, th2_hd));
    printf("Error\n");
}
/**
 * @brief 启动两个线程并进行ipc测试
 *
 */
void map_test(void)
{
    th1_hd = handler_alloc();
    assert(th1_hd != HANDLER_INVALID);
    th2_hd = handler_alloc();
    assert(th2_hd != HANDLER_INVALID);

    msg_tag_t tag;
    tag = factory_create_thread(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, th1_hd));
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_msg_buf_set(th1_hd, msg_buf0);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_exec_regs(th1_hd, (umword_t)thread_test_func, (umword_t)stack0 + STACK_SIZE, RAM_BASE(), 0);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_bind_task(th1_hd, TASK_THIS);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_run(th1_hd, 2);

    assert(msg_tag_get_prot(tag) >= 0);
    tag = factory_create_thread(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, th2_hd));
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_msg_buf_set(th2_hd, msg_buf1);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_exec_regs(th2_hd, (umword_t)thread_test_func2, (umword_t)stack1 + STACK_SIZE, RAM_BASE(), 0);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_bind_task(th2_hd, TASK_THIS);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_run(th2_hd, 2);
    assert(msg_tag_get_prot(tag) >= 0);
}
