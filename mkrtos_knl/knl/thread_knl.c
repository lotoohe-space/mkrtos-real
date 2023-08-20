/*
 * @Author: zhangzheng 1358745329@qq.com
 * @Date: 2023-08-14 09:47:54
 * @LastEditors: zhangzheng 1358745329@qq.com
 * @LastEditTime: 2023-08-18 16:21:58
 * @FilePath: /mkrtos-real/mkrtos_knl/knl/thread_knl.c
 * @Description: 内核线程初始化
 */

#include "types.h"
#include "init.h"
#include "printk.h"
#include "task.h"
#include "thread.h"
#include "factory.h"
#include "globals.h"
#include "arch.h"

static thread_t *knl_thread;
static task_t knl_task;

/**
 * 初始化内核线程
 * 初始化内核任务
 */
INIT_STAGE1 static void knl_init_1(void)
{
    thread_t *cur_th = thread_get_current();

    thread_init(cur_th);
    task_init(&knl_task, &root_factory_get()->limit);

    thread_bind(cur_th, &knl_task.kobj);
}
/**
 * 初始化init线程
 * 初始化用户态任务
 * 映射静态内核对象
 * 运行init进程
 */
INIT_STAGE2 static void knl_init_2(void)
{
    thread_t *init_thread = thread_create(&root_factory_get()->limit);
    assert(init_thread);
}
void start_kernel(void)
{
    // 初始化系统时钟
    // 初始化串口
    // 初始化定时器
    sys_call_init();
    printk("mkrtos init done..\n");
    printk("mkrtos running..\n");
    thread_ready(thread_get_current(), FALSE);
    sti();
    sys_startup();
    thread_sched();
    cli();

    while (1)
        ;
}
