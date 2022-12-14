#include <arch/atomic.h>
#include <mkrtos/sched.h>
#include "config.h"
#include "knl_service.h"
#include <mm.h>

static void test_thread_entry(int arg0, int arg1)
{
    struct task *cur_task;
    
    cur_task = get_current_task();
    MKRTOS_ASSERT(arg0 == 1);
    MKRTOS_ASSERT(arg1 == 2);
    //kprint("pid:%d\n", cur_task->pid);
}
static void test_thread_prio(int arg0, int arg1)
{
    struct task *cur_task;
    
    cur_task = get_current_task();
    MKRTOS_ASSERT(arg0 == 1);
    MKRTOS_ASSERT(arg1 == 2);
    sleep_ms(50);
    kprint("pid:%d prio:%d\n", cur_task->pid, cur_task->prio);
}
static int test_thread(void)
{
    pid_t pid;
    static struct task_create_par tcp;

    kprint("=======start sched mem\n");
    for (int i = 0; i < 100; i++) {
        tcp.task_fun = test_thread_entry;
        tcp.arg0 = (void*) 1;
        tcp.arg1 = 2;
        tcp.prio = 6;
        tcp.exec_id = -1;
        tcp.user_stack_size = 0;
        tcp.kernel_stack_size = 256;
        tcp.task_name = "main";

        pid = task_create(&tcp, TRUE);
        if (pid < 0) {
            sleep_ms(50);
            continue;
        }
    }
    while(atomic_read(&sys_tasks_info.wait_r)!=0) {
        sleep_ms(500);
    }
    sleep_ms(500);
    sche_lock();
    for (int i = 1; i <= 3; i++) {
        tcp.task_fun = test_thread_prio;
        tcp.arg0 = (void*) 1;
        tcp.arg1 = 2;
        tcp.prio = i;
        tcp.exec_id = -1;
        tcp.user_stack_size = 0;
        tcp.kernel_stack_size = 256;
        tcp.task_name = "main";

        pid = task_create(&tcp, TRUE);
        if (pid < 0) {
            kprint("=======prio test fail.\n");
            break;
        }
    }
    sche_unlock();
    while(atomic_read(&sys_tasks_info.wait_r)!=0) {
        sleep_ms(500);
    }
    sleep_ms(1000);
    knl_mem_trace();
    kprint("=======end sched mem\n");
    return 0;
}

INIT_TEST_REG(test_thread, INIT_TEST_LVL);
