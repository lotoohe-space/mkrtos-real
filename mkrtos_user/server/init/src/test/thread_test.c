#include "u_log.h"
#include "u_prot.h"
#include "u_mm.h"
#include "u_factory.h"
#include "u_thread.h"
#include "u_task.h"
#include "u_ipc.h"
#include <assert.h>
#include <stdio.h>

static char msg_buf0[MSG_BUG_LEN];
static char msg_buf1[MSG_BUG_LEN];
#define STACK_SIZE 1024
static __attribute__((aligned(8))) uint8_t stack0[STACK_SIZE];
static __attribute__((aligned(8))) uint8_t stack1[STACK_SIZE];
static void thread_test_func(void)
{
    char *buf;
    umword_t len;
    thread_msg_buf_get(11, (umword_t *)(&buf), NULL);
    while (1)
    {
        msg_tag_t tag = ipc_recv(12);
        if (msg_tag_get_prot(tag) > 0)
        {
            buf[msg_tag_get_prot(tag)] = 0;
            printf("recv data is %s\n", buf);
        }
        strcpy(buf, "reply");
        ipc_send(12, strlen("reply"), 0);
    }
    printf("thread_test_func.\n");
    task_unmap(TASK_PROT, 11);
    printf("Error\n");
}
static void thread_test_func2(void)
{
    char *buf;
    umword_t len;
    thread_msg_buf_get(10, (umword_t *)(&buf), NULL);
    while (1)
    {
        strcpy(buf, "1234");
        ipc_send(12, strlen(buf), 0);
        msg_tag_t tag = ipc_recv(12);
        if (msg_tag_get_prot(tag) > 0)
        {
            buf[msg_tag_get_prot(tag)] = 0;
            printf("recv data is %s\n", buf);
        }
    }
    printf("thread_test_func2.\n");
    task_unmap(TASK_PROT, 10);
    printf("Error\n");
}
/**
 * @brief 启动两个线程并进行ipc测试
 *
 */
void thread_test(void)
{
    msg_tag_t tag = factory_create_ipc(FACTORY_PROT, 12);
    if (msg_tag_get_prot(tag) < 0)
    {
        printf("factory_create_ipc no memory\n");
        return;
    }
    tag = factory_create_thread(FACTORY_PROT, 11);

    if (msg_tag_get_prot(tag) < 0)
    {
        printf("factory_create_thread no memory\n");
        return;
    }
    thread_msg_buf_set(11, msg_buf0);
    thread_exec_regs(11, (umword_t)thread_test_func, (umword_t)stack0 + STACK_SIZE, RAM_BASE());
    thread_bind_task(11, TASK_PROT);
    thread_run(11);

    factory_create_thread(FACTORY_PROT, 10);
    thread_msg_buf_set(10, msg_buf1);
    thread_exec_regs(10, (umword_t)thread_test_func2, (umword_t)stack1 + STACK_SIZE, RAM_BASE());
    thread_bind_task(10, TASK_PROT);
    thread_run(10);
}
