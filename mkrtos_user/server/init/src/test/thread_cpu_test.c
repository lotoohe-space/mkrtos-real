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
#include "u_sleep.h"
#include "u_vmam.h"
#include <string.h>
#include <stdlib.h>
static umword_t th1_hd = 0;
static umword_t th2_hd = 0;

#define STACK_SIZE 4096
static __attribute__((aligned(8))) uint8_t stack0[STACK_SIZE];
static __attribute__((aligned(8))) uint8_t stack1[STACK_SIZE];

#if IS_ENABLED(CONFIG_MMU)
//! 读取系统寄存器
#define read_sysreg(reg) ({      \
    unsigned long _val;          \
    asm volatile("mrs %0, " #reg \
                 : "=r"(_val));  \
    _val;                        \
})
static inline int arch_get_current_cpu_id(void)
{
    return read_sysreg(mpidr_el1) & 0XFFUL;
}
#else
static inline int arch_get_current_cpu_id(void)
{
    return 0;
}
#endif
static void thread_test_func(void)
{
    u_sleep_ms(500);
    printf("[u]thread1 run %d cpu.\n", arch_get_current_cpu_id());
    while (1)
    {
        thread_run_cpu(th1_hd, 2, rand() % (CONFIG_CPU));
        u_sleep_ms(5); /*TODO:*/
        printf("[u]thread1 run %d cpu.\n", arch_get_current_cpu_id());
    }
    handler_free_umap(th1_hd);
    printf("Error\n");
}
static void thread_test_func2(void)
{
    u_sleep_ms(500);
    while (1)
    {
        thread_run_cpu(th2_hd, 2, rand() % (CONFIG_CPU));
        u_sleep_ms(5); /*TODO:*/
        printf("[u]thread2 run %d cpu.\n", arch_get_current_cpu_id());
    }
    handler_free_umap(th2_hd);
    printf("Error\n");
}
void thread_cpu_test(void)
{
    th1_hd = handler_alloc();
    assert(th1_hd != HANDLER_INVALID);
    msg_tag_t tag;
    void *mem;

    tag = factory_create_thread(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, th1_hd));
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_bind_task(th1_hd, TASK_THIS);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_exec_regs(th1_hd, (umword_t)thread_test_func, (umword_t)stack0 + STACK_SIZE - 8, TASK_RAM_BASE(), 0);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = u_vmam_alloc(VMA_PROT, vma_addr_create(VPAGE_PROT_RW, 0, 0), PAGE_SIZE, 0, (addr_t *)(&mem));
    assert(msg_tag_get_prot(tag) >= 0);
    memset((void *)mem, 0, PAGE_SIZE);
    // 设置msgbuff
    tag = thread_msg_buf_set(th1_hd, mem);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_run_cpu(th1_hd, 2, 3);

    th2_hd = handler_alloc();
    assert(th2_hd != HANDLER_INVALID);

    tag = factory_create_thread(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, th2_hd));
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_bind_task(th2_hd, TASK_THIS);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_exec_regs(th2_hd, (umword_t)thread_test_func2, (umword_t)stack1 + STACK_SIZE - 8, TASK_RAM_BASE(), 0);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = u_vmam_alloc(VMA_PROT, vma_addr_create(VPAGE_PROT_RW, 0, 0), PAGE_SIZE, 0, (addr_t *)(&mem));
    assert(msg_tag_get_prot(tag) >= 0);
    memset((void *)mem, 0, PAGE_SIZE);
    // 设置msgbuff
    tag = thread_msg_buf_set(th2_hd, mem);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_run_cpu(th2_hd, 2, 2);
}
