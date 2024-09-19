#include "u_log.h"
#include "u_prot.h"

#include "u_factory.h"
#include "u_thread.h"
#include "u_task.h"
#include "u_ipc.h"
#include "u_hd_man.h"
#include "u_task.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <u_sleep.h>
#include <stdlib.h>

#define DEBUG_IPC_CALL 1

#if IS_ENABLED(CONFIG_MMU)
//! 读取系统寄存器
#define read_sysreg(reg) ({      \
    unsigned long _val;          \
    asm volatile("mrs %0, " #reg \
                 : "=r"(_val));  \
    _val;                        \
})
static inline int get_cpu_id(void)
{
    return read_sysreg(mpidr_el1) & 0xff;	// 读取该寄存器获取处理器id
}
#else
static inline int get_cpu_id(void)
{
    return 0;
}
#endif
static umword_t th1_hd = 0;
static umword_t th2_hd = 0;
static umword_t th3_hd = 0;
static umword_t th4_hd = 0;

static __attribute__((aligned(4096))) char msg_buf0[4096];
static __attribute__((aligned(4096))) char msg_buf1[4096];
static __attribute__((aligned(4096))) char msg_buf2[4096];
static __attribute__((aligned(4096))) char msg_buf3[4096];
#define STACK_SIZE 2048
static __attribute__((aligned(8))) uint8_t stack0[STACK_SIZE];
static __attribute__((aligned(8))) uint8_t stack1[STACK_SIZE];
static __attribute__((aligned(8))) uint8_t stack2[STACK_SIZE];
static __attribute__((aligned(8))) uint8_t stack3[STACK_SIZE];

static void hard_sleep(void)
{

    for (volatile int i; i < 10000000; i++)
        ;
}
static int test_cn = 0;
static int to_cpu[4];

static void thread_test_func(void)
{
    char *buf;
    umword_t len;

    printf("%s:%d thread 0 init.\n", __func__, __LINE__);
    while (1)
    {
        
        to_cpu[0] = rand() % 4;

        printf("thread 0 to %d cpu.\n", to_cpu[0]);
        thread_run_cpu(th1_hd, 2, to_cpu[0]);
        assert(to_cpu[0] == get_cpu_id());

        test_cn++;
    }
}
static void thread_test_func2(void)
{
    char *buf;
    umword_t len;

    printf("%s:%d thread 1 init.\n", __func__, __LINE__);
    while (1)
    {
        to_cpu[1] = rand() % 4;

        printf("thread 1 to %d cpu.\n", to_cpu[1]);
        thread_run_cpu(th2_hd, 2, to_cpu[1]);
        assert(to_cpu[1] == get_cpu_id());

        test_cn++;

    }
}

static void thread_test_func3(void)
{

    printf("%s:%d thread 2 init.\n", __func__, __LINE__);
    while (1)
    {
        to_cpu[2] = rand() % 4;

        printf("thread 2 to %d cpu.\n", to_cpu[2]);
        thread_run_cpu(th3_hd, 2, to_cpu[2]);
        assert(to_cpu[2] == get_cpu_id());

        test_cn++;

    }
}
static void thread_test_func4(void)
{

    printf("%s:%d thread 3 init.\n", __func__, __LINE__);
    while (1)
    {
        to_cpu[3] = rand() % 4;

        printf("thread 3 to %d cpu.\n", to_cpu[3]);
        thread_run_cpu(th4_hd, 2, to_cpu[3]);
        assert(to_cpu[3] == get_cpu_id());

        test_cn++;

    }
}
/**
 * @brief 启动两个线程并进行ipc测试
 *
 */
void ipi_test(void)
{
    msg_tag_t tag;
    th1_hd = handler_alloc();
    assert(th1_hd != HANDLER_INVALID);
    th2_hd = handler_alloc();
    assert(th2_hd != HANDLER_INVALID);
    th3_hd = handler_alloc();
    assert(th3_hd != HANDLER_INVALID);

    memset(msg_buf0, 0, sizeof(msg_buf0));
    memset(msg_buf1, 0, sizeof(msg_buf1));
    memset(msg_buf2, 0, sizeof(msg_buf2));
    memset(msg_buf3, 0, sizeof(msg_buf3));

    tag = factory_create_thread(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, th1_hd));
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_bind_task(th1_hd, TASK_THIS);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_msg_buf_set(th1_hd, msg_buf0);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_exec_regs(th1_hd, (umword_t)thread_test_func, (umword_t)stack0 + STACK_SIZE, TASK_RAM_BASE(), 0);
    task_set_obj_name(TASK_THIS, th1_hd, "cli_th");
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_run_cpu(th1_hd, 2, 0);
    assert(msg_tag_get_prot(tag) >= 0);

    assert(msg_tag_get_prot(tag) >= 0);
    tag = factory_create_thread(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, th2_hd));
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_bind_task(th2_hd, TASK_THIS);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_msg_buf_set(th2_hd, msg_buf1);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_exec_regs(th2_hd, (umword_t)thread_test_func2, (umword_t)stack1 + STACK_SIZE, TASK_RAM_BASE(), 0);
    assert(msg_tag_get_prot(tag) >= 0);
    task_set_obj_name(TASK_THIS, th2_hd, "cli0_th");
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_run_cpu(th2_hd, 2, 0);

    tag = factory_create_thread(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, th3_hd));
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_bind_task(th3_hd, TASK_THIS);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_msg_buf_set(th3_hd, msg_buf2);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_exec_regs(th3_hd, (umword_t)thread_test_func3, (umword_t)stack2 + STACK_SIZE, TASK_RAM_BASE(), 0);
    assert(msg_tag_get_prot(tag) >= 0);
    task_set_obj_name(TASK_THIS, th3_hd, "cli1_th");
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_run_cpu(th3_hd, 2, 0);

    tag = factory_create_thread(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, th4_hd));
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_bind_task(th4_hd, TASK_THIS);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_msg_buf_set(th4_hd, msg_buf3);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_exec_regs(th4_hd, (umword_t)thread_test_func4, (umword_t)stack3 + STACK_SIZE, TASK_RAM_BASE(), 0);
    assert(msg_tag_get_prot(tag) >= 0);
    task_set_obj_name(TASK_THIS, th4_hd, "cli2_th");
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_run_cpu(th4_hd, 2, 0);

    while (test_cn < 1000)
        ;
    // task_unmap(TASK_THIS, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, th1_hd));
    // task_unmap(TASK_THIS, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, th2_hd));
    // task_unmap(TASK_THIS, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, th3_hd));
    // task_unmap(TASK_THIS, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, th4_hd));
    
}
