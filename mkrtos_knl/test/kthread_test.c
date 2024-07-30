
#include <task.h>
#include <thread.h>
#include <factory.h>
#include <thread_task_arch.h>

static void th_test(void *arg)
{
    int a[20];
    for (int i = 0; i < 20; i++)
    {
        a[i] = i;
    }
    while (1)
    {
        for (int i = 0; i < 20; i++)
        {
            printk("%d ", a[i]);
        }
        printk("\n");
        thread_sched(TRUE);
    }
}
static void th_test2(void *arg)
{
    int a[20];
    for (int i = 0; i < 20; i++)
    {
        a[i] = i;
    }
    while (1)
    {
        for (int i = 0; i < 20; i++)
        {
            printk("%d,", a[i]);
        }
        printk("\n");
        thread_sched(TRUE);
    }
}
void kthread_test_init(void)
{
    thread_t *thread2;
    task_t *cur_tk = thread_get_current_task();

    thread2 = thread_create(&root_factory_get()->limit, 0);
    assert(thread2);
    thread_bind(thread2, &cur_tk->kobj);
    thread_knl_pf_set(thread2, th_test);
    thread2->sche.prio=2;
    thread_ready(thread2, TRUE);

    thread_t *thread3;

    thread3 = thread_create(&root_factory_get()->limit, 0);
    assert(thread3);
    thread_bind(thread3, &cur_tk->kobj);
    thread_knl_pf_set(thread3, th_test2);
    thread3->sche.prio=2;
    thread_ready(thread3, TRUE);
}