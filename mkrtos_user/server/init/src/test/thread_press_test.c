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
#include "test.h"
static umword_t th1_hd = 0;
static umword_t ipc_hd = 0;
static int i = 100;

#define STACK_SIZE 512
static __attribute__((aligned(8))) uint8_t stack0[STACK_SIZE];

static void thread_test_func(void)
{
    // char *buf;
    // umword_t len;
    // thread_msg_buf_get(th1_hd, (umword_t *)(&buf), NULL);
    // printf("thread_test_func.\n");
    ulog_write_str(LOG_PROT, ".");
    // task_unmap(TASK_PROT, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, th1_hd));
    handler_free_umap(ipc_hd);
    handler_free_umap(th1_hd);
    printf("Error\n");
}

void thread_press_test(void)
{
    while (i--)
    {
        th1_hd = handler_alloc();
        assert(th1_hd != HANDLER_INVALID);
        ipc_hd = handler_alloc();
        assert(ipc_hd != HANDLER_INVALID);

        msg_tag_t tag = factory_create_ipc(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, ipc_hd));
        assert(msg_tag_get_prot(tag) >= 0);
        tag = factory_create_thread(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, th1_hd));
        assert(msg_tag_get_prot(tag) >= 0);
        tag = thread_exec_regs(th1_hd, (umword_t)thread_test_func, (umword_t)stack0 + STACK_SIZE, RAM_BASE(), 0);
        assert(msg_tag_get_prot(tag) >= 0);
        tag = thread_bind_task(th1_hd, TASK_THIS);
        assert(msg_tag_get_prot(tag) >= 0);
        tag = thread_run(th1_hd, 2);
        ulog_write_str(LOG_PROT, "\n");

        // sleep_tick(20);
    }
}
