

#include <scheduler.h>
#include <types.h>
#include <thread.h>
#include <util.h>
#include <atomics.h>
/**
 * @brief 调度函数
 *
 * @param usp
 * @param ksp
 * @param sp_type
 * @return sp_info_t*
 */
sp_info_t *schde_to(void *usp, void *ksp, umword_t sp_type)
{
    // printk("cur_th:0x%lx\n", thread_get_current());
    scheduler_t *sche = scheduler_get_current();

    sched_t *next = sche->cur_sche;
    assert(next);
    thread_t *next_th = container_of(next, thread_t, sche);

    assert(next_th->magic == THREAD_MAGIC);

    thread_t *cur_th = thread_get_current();
    if (sche->sched_reset == 1)
    {
        cur_th->sp.knl_sp = ksp;
        cur_th->sp.user_sp = usp;
        cur_th->sp.sp_type = sp_type;
    }
    else if (sche->sched_reset == 2)
    {
        cur_th->sp.knl_sp = ksp;
        // cur_th->sp.user_sp = usp;
        // cur_th->sp.sp_type = sp_type;
    }
    atomic_inc(&cur_th->time_count);

    sche->sched_reset = 1;
    return &next_th->sp;
}
