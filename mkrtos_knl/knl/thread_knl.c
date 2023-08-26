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
#include "map.h"
#include "app.h"
#include "mm_wrap.h"
#include "thread_armv7m.h"
#include "misc.h"
static thread_t *knl_thread;
static task_t knl_task;

/**
 * 初始化内核线程
 * 初始化内核任务
 */
static void knl_init_1(void)
{
    thread_t *cur_th = thread_get_current();

    thread_init(cur_th);
    task_init(&knl_task, &root_factory_get()->limit);

    thread_bind(cur_th, &knl_task.kobj);
    thread_ready(cur_th, FALSE);
}
INIT_STAGE1(knl_init_1);

/**
 * 初始化init线程
 * 初始化用户态任务
 * 映射静态内核对象
 * 运行init进程
 */
static void knl_init_2(void)
{
    thread_t *init_thread = thread_create(&root_factory_get()->limit);
    assert(init_thread);
    task_t *init_task = task_create(&root_factory_get()->limit);
    assert(init_task);

    app_info_t *app = app_info_get((void *)(KNL_TEXT + INIT_OFFSET));
    // 申请init的ram内存
    void *ram = mpu_ram_alloc(&init_task->mm_space, &root_factory_get()->limit, app->i.ram_size);
    if (!ram)
    {
        printk("申请init进程内存失败.\n");
        assert(ram);
    }
    mm_space_add(&init_task->mm_space, KNL_TEXT, 64*1024*1024, REGION_RO);
    void *sp_addr = (char *)ram + app->i.stack_offset - app->i.data_offset;
    void *sp_addr_top = (char *)sp_addr + app->i.stack_size;

    thread_bind(init_thread, &init_task->kobj);
    thread_user_pf_set(init_thread, (void *)(KNL_TEXT + INIT_OFFSET), sp_addr_top, ram);
    assert(obj_map_root(&init_thread->kobj, &init_task->obj_space, &root_factory_get()->limit, THREAD_PROT));
    assert(obj_map_root(&init_task->kobj, &init_task->obj_space, &root_factory_get()->limit, TASK_PROT));
    for (int i = FACTORY_PORT_START; i < FACTORY_PORT_END; i++)
    {
        kobject_t *kobj = global_get_kobj(i);
        if (kobj)
        {
            assert(obj_map_root(kobj, &init_task->obj_space, &root_factory_get()->limit, i));
        }
    }
    thread_ready(init_thread, FALSE);
}
INIT_STAGE1(knl_init_2);

static void print_mkrtos_info(void)
{
    const char *start_info[] = {
        " _____ ______   ___  __    ________  _________  ________  ________      \r\n",
        "|\\   _ \\  _   \\|\\  \\|\\  \\ |\\   __  \\|\\___   ___\\\\   __  \\|\\   ____\\     \r\n",
        "\\ \\  \\\\\\__\\ \\  \\ \\  \\/  /|\\ \\  \\|\\  \\|___ \\  \\_\\ \\  \\|\\  \\ \\  \\___|_    \r\n",
        " \\ \\  \\\\|__| \\  \\ \\   ___  \\ \\   _  _\\   \\ \\  \\ \\ \\  \\\\\\  \\ \\_____  \\   \r\n",
        "  \\ \\  \\    \\ \\  \\ \\  \\\\ \\  \\ \\  \\\\  \\|   \\ \\  \\ \\ \\  \\\\\\  \\|____|\\  \\  \r\n",
        "   \\ \\__\\    \\ \\__\\ \\__\\\\ \\__\\ \\__\\\\ _\\    \\ \\__\\ \\ \\_______\\____\\_\\  \\ \r\n",
        "    \\|__|     \\|__|\\|__| \\|__|\\|__|\\|__|    \\|__|  \\|_______|\\_________\\\r\n",
        "                                                            \\|_________|\r\n",
        "Complie Time:" __DATE__ " " __TIME__ "\r\n",
    };
    for (umword_t i = 0; i < sizeof(start_info) / sizeof(void *); i++)
    {
        printk(start_info[i]);
    }
}
void start_kernel(void)
{
    // 初始化系统时钟
    // 初始化串口
    // 初始化定时器
    sys_call_init();
    printk("mkrtos init done..\n");
    printk("mkrtos running..\n");
    print_mkrtos_info();
    sti();
    sys_startup();
    thread_sched();
    cli();

    while (1)
        ;
}
