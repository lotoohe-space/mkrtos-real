
#include "u_log.h"
#include "u_prot.h"
#include "u_mm.h"
#include "u_factory.h"
#include "u_thread.h"
#include "u_task.h"
#include <assert.h>
#include <stdio.h>
void ulog_test(void)
{
    ulog_write_str(LOG_PROT, "Init task running..\n");
    ulog_write_str(LOG_PROT, "todo..\n");
}
void mm_test(void)
{
    void *mem = mm_alloc_page(MM_PROT, 2, REGION_RWX);
    assert(mem);
    memset((char *)mem, 0, 1024);
    void *mem1 = mm_alloc_page(MM_PROT, 2, REGION_RWX);
    assert(mem1);
    memset(mem1, 0, 1024);

    mm_free_page(MM_PROT, mem1, 2);
    mm_free_page(MM_PROT, mem, 2);
    // memset(mem, 0, 512);
    // memset(mem1, 0, 512);
}
void thread_test_func(void)
{
    printf("thread_test_func.\n");
    task_unmap(TASK_PROT, 11);
    printf("Error\n");
}
void thread_test_func2(void)
{
    printf("thread_test_func2.\n");
    task_unmap(TASK_PROT, 10);
    printf("Error\n");
}
static __attribute__((aligned(8))) uint8_t val[1024];
static __attribute__((aligned(8))) uint8_t val1[1024];

typedef struct app_info
{
    const char d[32];
    const char magic[8];
    union
    {
        struct exec_head_info
        {
            umword_t ram_size;
            umword_t heap_offset;
            umword_t stack_offset;
            umword_t heap_size;
            umword_t stack_size;
            umword_t data_offset;
            umword_t bss_offset;
            umword_t got_start;
            umword_t got_end;
            umword_t rel_start;
            umword_t rel_end;
            umword_t text_start;
        } i;
        const char d1[128];
    };
    const char dot_text[];
} app_info_t;

void factory_test(void)
{
    // void *mem = mm_alloc_page(MM_PROT, 4, REGION_RWX);
    // assert(mem);
    // memset(mem, 0, 2048);
    msg_tag_t tag = factory_create_thread(FACTORY_PROT, 11);

    if (msg_tag_get_prot(tag) < 0)
    {
        printf("factory_create_thread no memory\n");
        return;
    }
#if 0
    thread_exec_regs(11, (umword_t)thread_test_func, (umword_t)val + 1024, RAM_BASE());
    thread_bind_task(11, TASK_PROT);
    thread_run(11);

    factory_create_thread(FACTORY_PROT, 10);
    thread_exec_regs(10, (umword_t)thread_test_func2, (umword_t)val1 + 1024, RAM_BASE());
    thread_bind_task(10, TASK_PROT);
    thread_run(10);
#else
    app_info_t *app = (app_info_t *)0x8014000;
    umword_t ram_base;
    tag = factory_create_task(FACTORY_PROT, 10);
    if (msg_tag_get_prot(tag) < 0)
    {
        printf("factory_create_task no memory\n");
        return;
    }
    tag = task_alloc_ram_base(10, app->i.ram_size, &ram_base);
    if (msg_tag_get_prot(tag) < 0)
    {
        printf("task_alloc_ram_base no memory\n");
        return;
    }
    task_map(10, LOG_PROT, LOG_PROT);
    void *sp_addr = (char *)ram_base + app->i.stack_offset - app->i.data_offset;
    void *sp_addr_top = (char *)sp_addr + app->i.stack_size;
    thread_exec_regs(11, (umword_t)0x8014000, (umword_t)sp_addr_top, ram_base);
    thread_bind_task(11, 10);
    thread_run(11);
#endif
}
void mpu_test(void)
{

#if 0
    // mpu保护测试
    int *test = ((int *)0x8000000);
    *test = 1;
    printf("test is %d\n", *test);
#endif
}
void printf_test(void)
{
    printf("print test0.\n");
    printf("print test1.\n");
    printf("print test2.\n");
    float a = 1.1;
    float b = 1.2;
    float c = a + b;
    // printf("%c %d %f\n", 'a', 1234, 1.1);
    // c = c;
    // printf("%c %d %lf\n", 'a', 1234, 1.1); 浮点打印有问题
}
int main(int argc, char *args[])
{
    ulog_write_str(LOG_PROT, "init..\n");
    factory_test();
#if 0
    mm_test();
    mpu_test();
    ulog_test();
#endif
    printf_test();
    printf("exit init.\n");
    while (1)
        ;
    // task_unmap(TASK_PROT, TASK_PROT);
    return 0;
}
