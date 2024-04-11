

#include <scheduler.h>
#include <types.h>
#include <thread.h>
#include <util.h>
sp_info_t *schde_to(void *usp, void *ksp, umword_t sp_type)
{
    scheduler_t *sche = scheduler_get_current();

    sched_t *next = sche->cur_sche;
    thread_t *next_th = container_of(next, thread_t, sche);

    assert(next_th->magic == THREAD_MAGIC);

    if (sched_reset)
    {
        thread_t *cur_th = thread_get_current();
        cur_th->sp.knl_sp = ksp;
        cur_th->sp.user_sp = usp;
        cur_th->sp.sp_type = sp_type;
    }
    sched_reset = 1;
    return &next_th->sp;
}
