#include "u_log.h"
#include "u_prot.h"

#include "u_factory.h"
#include "u_thread.h"
#include "u_task.h"
#include "u_ipc.h"
#include "u_hd_man.h"
#include <assert.h>
#include <stdio.h>
#include "test.h"
#include "u_sleep.h"
#include <cons_cli.h>
static umword_t th1_hd = 0;
static int i = 100;

#define STACK_SIZE 1024
static __attribute__((aligned(8))) uint8_t stack0[STACK_SIZE];

static void thread_test_func(void)
{
    cons_write_str(".\n");
    handler_free_umap(th1_hd);
    assert(0);
}
AUTO_CALL(102)
void thread_press_test(void)
{
    while (i--)
    {
        th1_hd = handler_alloc();
        assert(th1_hd != HANDLER_INVALID);
        msg_tag_t tag;

        tag = factory_create_thread(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, th1_hd));
        assert(msg_tag_get_prot(tag) >= 0);
        tag = thread_exec_regs(th1_hd, (umword_t)thread_test_func, (umword_t)stack0 + STACK_SIZE, TASK_RAM_BASE(), 0);
        assert(msg_tag_get_prot(tag) >= 0);
        tag = thread_bind_task(th1_hd, TASK_THIS);
        assert(msg_tag_get_prot(tag) >= 0);
        tag = thread_run(th1_hd, 2);
        u_sleep_ms(30);
    }
}
