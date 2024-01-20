/**
 * @file thread_knl.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-09-29
 *
 * @copyright Copyright (c) 2023
 *
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

static uint8_t knl_msg_buf[THREAD_MSG_BUG_LEN];
static thread_t *knl_thread;
static task_t knl_task;
static thread_t *init_thread;
static task_t *init_task;

static slist_head_t del_task_head;
static spinlock_t del_lock;

static void knl_main(void)
{
    umword_t status;
    umword_t status2;
    // printk("knl main run..\n");
    while (1)
    {
        task_t *pos;

        if (slist_is_empty(&del_task_head))
        {
            continue;
        }

        status2 = spinlock_lock(&del_lock);
        if (slist_is_empty(&del_task_head))
        {
            spinlock_set(&del_lock, status2);
            continue;
        }
        slist_foreach_not_next(pos, &del_task_head, del_node)
        {
            task_t *next = slist_next_entry(pos, &del_task_head, del_node);
            slist_del(&pos->del_node);
            {
                msg_tag_t tag;
                umword_t user_id;
                ipc_msg_t *msg = (ipc_msg_t *)knl_msg_buf;
                msg->msg_buf[0] = 1; /*KILL_TASK*/
                msg->msg_buf[1] = pos->pid;
                msg->msg_buf[2] = 0;
                int ret = thread_ipc_call(init_thread, msg_tag_init4(0, 3, 0, 0x0005 /*PM_PROT*/),
                                          &tag, ipc_timeout_create2(3000, 3000), &user_id, TRUE);

                if (ret < 0)
                {
                    printk("%s:%d ret:%d\n", __func__, __LINE__, ret);
                }
            }
            task_kill(pos);
            pos = next;
        }
        spinlock_set(&del_lock, status2);
    }
}
/**
 * 初始化内核线程
 * 初始化内核任务
 */
static void knl_init_1(void)
{
    knl_thread = thread_get_current();

    thread_init(knl_thread, &root_factory_get()->limit);
    task_init(&knl_task, &root_factory_get()->limit, TRUE);

    thread_knl_pf_set(knl_thread, knl_main);
    thread_bind(knl_thread, &knl_task.kobj);
    thread_set_msg_bug(knl_thread, knl_msg_buf);
    thread_ready(knl_thread, FALSE);

    slist_init(&del_task_head);
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
    mm_trace();


    init_thread = thread_create(&root_factory_get()->limit);
    assert(init_thread);
    init_task = task_create(&root_factory_get()->limit, FALSE);
    assert(init_task);

    app_info_t *app = app_info_get((void *)(CONFIG_KNL_TEXT_ADDR + CONFIG_INIT_TASK_OFFSET));
    // 申请init的ram内存
    assert(task_alloc_base_ram(init_task, &root_factory_get()->limit, app->i.ram_size + THREAD_MSG_BUG_LEN) >= 0);
    void *sp_addr = (char *)init_task->mm_space.mm_block + app->i.stack_offset - app->i.data_offset;
    void *sp_addr_top = (char *)sp_addr + app->i.stack_size;

    thread_set_msg_bug(init_thread, (char *)(init_task->mm_space.mm_block) + app->i.ram_size);
    thread_bind(init_thread, &init_task->kobj);
    thread_user_pf_set(init_thread, (void *)(CONFIG_KNL_TEXT_ADDR + CONFIG_INIT_TASK_OFFSET), (void *)((umword_t)sp_addr_top - 8),
                       init_task->mm_space.mm_block, 0);
    assert(obj_map_root(&init_thread->kobj, &init_task->obj_space, &root_factory_get()->limit, vpage_create3(KOBJ_ALL_RIGHTS, 0, THREAD_PROT)));
    assert(obj_map_root(&init_task->kobj, &init_task->obj_space, &root_factory_get()->limit, vpage_create3(KOBJ_ALL_RIGHTS, 0, TASK_PROT)));
    for (int i = FACTORY_PORT_START; i < FACTORY_PORT_END; i++)
    {
        kobject_t *kobj = global_get_kobj(i);
        if (kobj)
        {
            assert(obj_map_root(kobj, &init_task->obj_space, &root_factory_get()->limit, vpage_create3(KOBJ_ALL_RIGHTS, 0, i)));
        }
    }
    init_thread->sche.prio = 2;
    init_task->pid = 0;
    thread_ready(init_thread, FALSE);
}
INIT_STAGE2(knl_init_2);

void task_knl_kill(thread_t *kill_thread, bool_t is_knl)
{
    task_t *task = container_of(kill_thread->task, task_t, kobj);
    if (!is_knl)
    {
        printk("kill task:0x%x.\n", kill_thread->task);
        umword_t status2;

        thread_suspend(kill_thread);
        status2 = spinlock_lock(&del_lock);
        slist_add_append(&del_task_head, &task->del_node);
        spinlock_set(&del_lock, status2);
    }
    else
    {
        printk("[knl]: knl panic.\n");
        assert(0);
    }
}

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
    sys_startup(); //!< 开始调度
    thread_sched();
    cli();

    while (1)
        ;
    // printk(".");
}
