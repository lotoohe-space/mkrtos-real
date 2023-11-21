/*
 * 程序清单：完成量例程
 *
 * 程序会初始化 2 个线程及初始化一个完成量对象
 * 一个线程等待另一个线程发送完成量
 */
#include <rtthread.h>
#include <rtdevice.h>

#define THREAD_PRIORITY 9
#define THREAD_TIMESLICE 5

/* 完成量控制块 */
static struct rt_completion completion;

__attribute__((aligned(RT_ALIGN_SIZE))) static char thread1_stack[2048];
static struct rt_thread thread1;

/* 线程 1 入口函数 */
static void thread1_completion_wait(void *param)
{
    /* 等待完成 */
    rt_kprintf("thread1: completion is waitting\n");
    rt_completion_wait(&completion, RT_WAITING_FOREVER);
    rt_kprintf("thread1: completion waitting done\n");
    rt_kprintf("thread1 leave.\n");
}

__attribute__((aligned(RT_ALIGN_SIZE))) static char thread2_stack[2048];
static struct rt_thread thread2;

/* 线程 2 入口 */
static void thread2_completion_done(void *param)
{
    rt_kprintf("thread2: completion done\n");
    rt_completion_done(&completion);
    rt_kprintf("thread2 leave.\n");
}

int completion_sample(void)
{
    /* 初始化完成量对象 */
    rt_completion_init(&completion);

    rt_thread_init(&thread1,
                   "thread1",
                   thread1_completion_wait,
                   RT_NULL,
                   &thread1_stack[0],
                   sizeof(thread1_stack),
                   THREAD_PRIORITY - 1, THREAD_TIMESLICE);
    rt_thread_startup(&thread1);

    rt_thread_init(&thread2,
                   "thread2",
                   thread2_completion_done,
                   RT_NULL,
                   &thread2_stack[0],
                   sizeof(thread2_stack),
                   THREAD_PRIORITY, THREAD_TIMESLICE);
    rt_thread_startup(&thread2);

    return 0;
}
