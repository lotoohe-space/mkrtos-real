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
#define DEBUG_IPC_CALL 1

static pthread_mutex_t lock;
static pthread_t pth;
static pthread_t pth2;

#define STACK_SIZE (1024 + 256)
static __attribute__((aligned(8))) uint8_t stack0[STACK_SIZE];
static __attribute__((aligned(8))) uint8_t stack1[STACK_SIZE];

static void hard_sleep(void)
{
    for (volatile int i; i < 1000000000; i++)
        ;
}
static void *thread_test_func(void *arg)
{
    int i = 5;
    while (i--)
    {
        pthread_mutex_lock(&lock);
        printf("thread 1 ..\n");
        usleep(200000);
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}
static void *thread_test_func2(void *arg)
{
    int i = 5;
    while (i--)
    {
        pthread_mutex_lock(&lock);
        printf("thread 2 ..\n");
        usleep(200000);
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

/**
 *
 */
AUTO_CALL(102)
void pthread_lock_test(void)
{
    pthread_attr_t attr;

    pthread_mutex_init(&lock, NULL);
    pthread_attr_init(&attr);
    pthread_attr_setstack(&attr, stack0, STACK_SIZE);
    pthread_create(&pth, &attr, thread_test_func, NULL);
    pthread_attr_setstack(&attr, stack1, STACK_SIZE);
    pthread_create(&pth2, &attr, thread_test_func2, NULL);
    pthread_join(pth, NULL);
    pthread_join(pth2, NULL);
}
