
#include <task.h>
#include <thread.h>
#include <factory.h>
#include <thread_task_arch.h>
#include <kthread_test.h>
#include <buddy_slab_test.h>
void knl_test(void)
{
    kthread_test_init();
    buddy_slab_init();
}
