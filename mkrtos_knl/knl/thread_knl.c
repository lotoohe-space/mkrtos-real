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

#include "app.h"
#include "arch.h"
#include "factory.h"
#include "globals.h"
#include "init.h"
#include "knl_misc.h"
#include "map.h"
#include "mm_wrap.h"
#include "mpu.h"
#include "printk.h"
#include "task.h"
#include "thread.h"
#include "thread_task_arch.h"
#include "types.h"
#include "prot.h"
#include <assert.h>
#if IS_ENABLED(CONFIG_KNL_TEST)
#include <knl_test.h>
#endif
#if ARCH_WORD_SIZE == 64
#include <elf64.h>
#endif
#if IS_ENABLED(CONFIG_MMU)
#include <vma.h>
#endif
#if IS_ENABLED(CONFIG_CPIO_SUPPORT)
#include <cpio.h>
#else
#include <appfs_tiny.h>
#endif
#include <boot_info.h>
#include "pre_cpu.h"

static uint8_t knl_msg_buf[CONFIG_CPU][THREAD_MSG_BUG_LEN];
static thread_fast_ipc_com_t knl_th_ipc_com;
static task_t knl_task;
static thread_t *init_thread;
static task_t *init_task;
static thread_t *knl_thread[CONFIG_CPU];
static slist_head_t del_task_head;
static umword_t cpu_usage[CONFIG_CPU];
static spinlock_t del_lock;
static umword_t cpu_usage_last_tick_val[CONFIG_CPU];

static void knl_main(void)
{
    umword_t status;
    umword_t status2;
    printk("knl main run..\n");
    while (1)
    {
        task_t *pos;

        if (slist_is_empty(&del_task_head))
        {
            cpu_sleep();
            continue;
        }

        status2 = spinlock_lock(&del_lock);
        if (slist_is_empty(&del_task_head))
        {
            spinlock_set(&del_lock, status2);
            continue;
        }
        // 在这里删除进程
        slist_foreach_not_next(pos, &del_task_head, del_node)
        {
            task_t *next = slist_next_entry(pos, &del_task_head, del_node);
            slist_del(&pos->del_node);
            {
                msg_tag_t tag;
                umword_t user_id;
                ipc_msg_t *msg = (ipc_msg_t *)knl_msg_buf;

                if (pos->pid != 0)
                {
                    msg->msg_buf[0] = 1; /*KILL_TASK*/
                    msg->msg_buf[1] = pos->pid;
                    msg->msg_buf[2] = 0;

                    if (thread_get_ipc_state(init_thread) != THREAD_IPC_ABORT)
                    {
#define PM_PROT 0x0005
#define MAGIC_NS_USERPID 0xbabababa
                        entry_frame_t f;
                        f.regs[0] = msg_tag_init4(0, 3, 0, PM_PROT).raw;
                        f.regs[1] = 0;
                        f.regs[2] = 0x2222; /*传递两个参数，没有用到，暂时用不上*/
                        f.regs[3] = 0x3333;
                        tag = thread_fast_ipc_call(init_task, &f, MAGIC_NS_USERPID);
                        if (msg_tag_get_val(tag) < 0)
                        {
                            printk("init thread comm failed, ret:%d\n", __func__, __LINE__, msg_tag_get_val(tag));
                        }
                    }
                }
            }
            task_kill(pos);
            pos = next;
        }
        spinlock_set(&del_lock, status2);
    }
}
static inline uint32_t thread_knl_get_current_run_nr(void)
{
    if (knl_thread[arch_get_current_cpu_id()] == NULL)
    {
        return 0;
    }
    return atomic_read(&knl_thread[arch_get_current_cpu_id()]->time_count);
}

/**
 * 计算cpu占用率
 */
void thread_calc_cpu_usage(void)
{
    uint8_t cur_cpu_id = arch_get_current_cpu_id();
    umword_t tick = thread_knl_get_current_run_nr();

    if (sys_tick_cnt_get() % 1000 == 0)
    {
        mword_t calc = tick - cpu_usage_last_tick_val[cur_cpu_id];

        calc = calc > 1000 ? 1000 : calc;
        cpu_usage[cur_cpu_id] = 1000 - calc;
        cpu_usage_last_tick_val[cur_cpu_id] = tick;
    }
}
uint16_t cpu_get_current_usage(void)
{
    return (uint16_t)cpu_usage[arch_get_current_cpu_id()];
}
/**
 * 初始化内核线程
 * 初始化内核任务
 */
void knl_init_1(void)
{
    thread_t *knl_th;

    knl_thread[arch_get_current_cpu_id()] = thread_get_current();
    knl_th = knl_thread[arch_get_current_cpu_id()];
    knl_th->com = &knl_th_ipc_com;
    thread_init(knl_th, &root_factory_get()->limit, FALSE);
    task_init(&knl_task, &root_factory_get()->limit, TRUE);
    task_knl_init(&knl_task);
    kobject_set_name(&knl_task.kobj, "tk_knl");
    thread_knl_pf_set(knl_th, knl_main);
    thread_bind(knl_th, &knl_task.kobj);
    kobject_set_name(&knl_th->kobj, "th_knl");
    thread_set_msg_buf(knl_th, knl_msg_buf[arch_get_current_cpu_id()],
                       knl_msg_buf[arch_get_current_cpu_id()]);
    knl_th->cpu = arch_get_current_cpu_id();
    thread_ready(knl_th, FALSE);
}
INIT_STAGE1(knl_init_1);

/**
 * 是否是内核线程
 */
bool_t thread_is_knl(thread_t *thread)
{
    if (thread == knl_thread[arch_get_current_cpu_id()])
        return TRUE;
    else
        return FALSE;
}

/**
 * 初始化init线程
 * 初始化用户态任务
 * 映射静态内核对象
 * 运行init进程
 */
static void knl_init_2(void)
{
    mm_trace();
    slist_init(&del_task_head);
#if IS_ENABLED(CONFIG_KNL_TEST)
    knl_test();
#else

    umword_t ret_addr;
    size_t size;

    init_thread = thread_create(&root_factory_get()->limit, FALSE);
    assert(init_thread);
    init_task = task_create(&root_factory_get()->limit, FALSE);
    assert(init_task);

#if IS_ENABLED(CONFIG_ELF_LAUNCH)
    addr_t entry;
#if IS_ENABLED(CONFIG_CPIO_SUPPORT)
    ret_addr = cpio_find_file(cpio_images, (umword_t)(-1), "init", &size);
#else
    ret_addr = appfs_find_file_addr_by_name(appfs_get_form_addr((void *)(cpio_images)), "init", &size);
#endif
    assert(ret_addr);
    elf_load(init_task, ret_addr, size, &entry);
    void *init_msg_buf = mm_buddy_alloc_one_page();
    assert(init_msg_buf);
    assert(task_vma_alloc(&init_task->mm_space.mem_vma,
                          vma_addr_create(VPAGE_PROT_RW, VMA_ADDR_RESV, CONFIG_MSG_BUF_VADDR),
                          PAGE_SIZE, (paddr_t)init_msg_buf, 0) >= 0);
    assert(task_vma_alloc(&init_task->mm_space.mem_vma,
                          vma_addr_create(VPAGE_PROT_RO, VMA_ADDR_RESV, CONFIG_BOOT_FS_VADDR),
                          cpio_get_size(cpio_images), (paddr_t)cpio_images, 0) >= 0);
    thread_set_msg_buf(init_thread, (void *)init_msg_buf, (void *)CONFIG_MSG_BUF_VADDR);
    thread_user_pf_set(init_thread, (void *)(entry), (void *)0xdeaddead,
                       NULL);
#else
    app_info_t *app;

#if IS_ENABLED(CONFIG_CPIO_SUPPORT)
    ret_addr = cpio_find_file(arch_get_boot_info()->flash_layer.flash_layer_list[BOOTFS_LAYER_3].st_addr, (umword_t)(-1), "init", &size);
#else
    ret_addr = appfs_find_file_addr_by_name(appfs_get_form_addr((void *)(arch_get_boot_info()->flash_layer.flash_layer_list[BOOTFS_LAYER_3].st_addr)), "init", &size);
#endif
    assert(ret_addr);
    app = app_info_get((void *)(ret_addr));
    assert(app);
    printk("init task text is 0x%x.\n", app);
    // 申请init的ram内存
    assert(task_alloc_base_ram(init_task, &root_factory_get()->limit, app->i.ram_size + THREAD_MSG_BUG_LEN, 0) >= 0);
    void *sp_addr = (char *)init_task->mm_space.mm_block + app->i.stack_offset - app->i.data_offset;
    void *sp_addr_top = (char *)sp_addr + app->i.stack_size;

    thread_set_msg_buf(init_thread, (char *)(init_task->mm_space.mm_block) + app->i.ram_size, (char *)(init_task->mm_space.mm_block) + app->i.ram_size);
    thread_user_pf_set(init_thread, (void *)(app), (void *)((umword_t)sp_addr_top - 8),
                       init_task->mm_space.mm_block);
#endif
    thread_bind(init_thread, &init_task->kobj);
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
    init_thread->sche.prio = 3;
    init_task->pid = 0;
    thread_ready(init_thread, FALSE);
#endif
}
INIT_STAGE2(knl_init_2);

bool_t task_knl_kill(thread_t *kill_thread, bool_t is_knl)
{
    bool_t reset_ram = FALSE;
    task_t *task = container_of(kill_thread->task, task_t, kobj);
    if (!is_knl)
    {
        printk("kill %s th:0x%x task:0x%x, pid:%d\n", kobject_get_name(&task->kobj), kill_thread, task, task->pid);
        umword_t status2;

        status2 = spinlock_lock(&del_lock);
        if (stack_len(&kill_thread->com->fast_ipc_stack) != 0)
        {
            // 在通信的时候出现了错误
            // fast_ipc需要测试场景
            // 1. 在ipc到其他进程中时其他进程死亡
            // 2. 在ipc到其他进程中时当前进程死亡
            int ret;
            thread_fast_ipc_item_t ipc_item;

            ret = thread_fast_ipc_restore(kill_thread);
            if (ret >= 0)
            {
                // 还原栈和usp FIXME:arch相关的
                thread_user_pf_restore(kill_thread, (void *)arch_get_user_sp());
                mpu_switch_to_task(thread_get_bind_task(kill_thread));
                ref_counter_dec_and_release(&task->ref_cn, &task->kobj);
                reset_ram = TRUE;
            }
        }
        else
        {
            thread_suspend(kill_thread);
            kill_thread->ipc_status = THREAD_IPC_ABORT;
        }
        slist_add_append(&del_task_head, &task->del_node);
        spinlock_set(&del_lock, status2);
    }
    else
    {
        printk("[knl]: knl panic.\n");
        assert(0);
    }
    return reset_ram;
}

static void print_mkrtos_info(void)
{
    static const char *start_info[] = {
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
void start_kernel(void *boot_info)
{
    // 初始化系统时钟
    // 初始化串口
    // 初始化定时器
#if IS_ENABLED(CONFIG_BOOT_INFO_SUPPORT)
    arch_set_boot_info(boot_info);
#endif
    sys_call_init();
    printk("mkrtos init done..\n");
    printk("mkrtos running..\n");
    printk("sys freq:%d\n", arch_get_sys_clk());
    print_mkrtos_info();
    cli();
    sys_startup(); //!< 开始调度
    thread_sched(FALSE);
    arch_to_sche();
    sti();

    while (1)
    {
        knl_main();
    }
}
