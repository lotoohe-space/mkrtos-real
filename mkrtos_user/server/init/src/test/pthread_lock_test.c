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
#define DEBUG_IPC_CALL 1

static pthread_mutex_t lock;
static pthread_t pth;
static pthread_t pth2;

#define STACK_SIZE 2048
static __attribute__((aligned(8))) uint8_t stack0[STACK_SIZE];
static __attribute__((aligned(8))) uint8_t stack1[STACK_SIZE];

static void hard_sleep(void)
{
    for (volatile int i; i < 10000000; i++)
        ;
}
static void *thread_test_func(void* arg)
{
    while (1)
    {
        pthread_mutex_lock(&lock);
        printf("thread 1 ..\n");
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}
static void *thread_test_func2(void* arg)
{
    while (1)
    {
        pthread_mutex_lock(&lock);
        printf("thread 2 ..\n");
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

/**
 *
 */
void pthread_lock_test(void)
{
    pthread_attr_t attr;

    pthread_mutex_init(&lock, NULL);
    pthread_attr_init(&attr);
    pthread_attr_setstack(&attr, stack0, STACK_SIZE);
    pthread_create(&pth, &attr, thread_test_func, NULL);
    pthread_attr_setstack(&attr, stack1, STACK_SIZE);
    pthread_create(&pth2, &attr, thread_test_func2, NULL);

    // th1_hd = handler_alloc();
    // assert(th1_hd != HANDLER_INVALID);
    // th2_hd = handler_alloc();
    // assert(th2_hd != HANDLER_INVALID);
    // ipc_hd = handler_alloc();
    // assert(ipc_hd != HANDLER_INVALID);

    // msg_tag_t tag = factory_create_ipc(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, ipc_hd));
    // assert(msg_tag_get_prot(tag) >= 0);
    // tag = factory_create_thread(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, th1_hd));
    // assert(msg_tag_get_prot(tag) >= 0);
    // ipc_bind(ipc_hd, th1_hd, 0);
    // tag = thread_msg_buf_set(th1_hd, msg_buf0);
    // assert(msg_tag_get_prot(tag) >= 0);
    // tag = thread_exec_regs(th1_hd, (umword_t)thread_test_func, (umword_t)stack0 + STACK_SIZE, RAM_BASE(), 0);
    // assert(msg_tag_get_prot(tag) >= 0);
    // tag = thread_bind_task(th1_hd, TASK_THIS);
    // assert(msg_tag_get_prot(tag) >= 0);
    // tag = thread_run(th1_hd, 2);

    // assert(msg_tag_get_prot(tag) >= 0);
    // tag = factory_create_thread(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, th2_hd));
    // assert(msg_tag_get_prot(tag) >= 0);
    // tag = thread_msg_buf_set(th2_hd, msg_buf1);
    // assert(msg_tag_get_prot(tag) >= 0);
    // tag = thread_exec_regs(th2_hd, (umword_t)thread_test_func2, (umword_t)stack1 + STACK_SIZE, RAM_BASE(), 0);
    // assert(msg_tag_get_prot(tag) >= 0);
    // tag = thread_bind_task(th2_hd, TASK_THIS);
    // assert(msg_tag_get_prot(tag) >= 0);
    // tag = thread_run(th2_hd, 2);
    // assert(msg_tag_get_prot(tag) >= 0);
}
