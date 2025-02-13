/**
 * @file sleep.c
 * @author your name (1358745329@qq.com)
 * @brief 线程休眠支持
 * @version 0.1
 * @date 2025-02-12
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <thread.h>
#include <types.h>
#include <spinlock.h>
#include <slist.h>
#include <init.h>
#include "sleep.h"
#include "arch.h"

static slist_head_t wait_list;
static spinlock_t lock;

static inline void thread_wait_entry_init(thread_wait_entry_t *entry,
                                          thread_t *th, mword_t times)
{
    slist_init(&entry->node);
    entry->th = th;
    entry->times = times;
}
static void thread_timeout_init(void)
{
    slist_init(&wait_list);
    spinlock_init(&lock);
}
INIT_KOBJ(thread_timeout_init);
/**
 * @brief  必须在中断中调用
 *
 */
void thread_check_timeout(void)
{
    thread_wait_entry_t *pos;

    slist_foreach_not_next(
        pos, (slist_head_t *)&wait_list,
        node)
    {
        thread_wait_entry_t *next = slist_next_entry(
            pos, (slist_head_t *)wait_list,
            node);
        if (pos->times != 0)
        {
            pos->times--;
            if (pos->times == 0)
            {
                assert(pos->th->status == THREAD_SUSPEND);
                thread_ready(pos->th, TRUE);
                slist_del(&pos->node);
            }
        } // !< 如果是0，则一直休眠
        pos = next;
    }
}
/**
 * @brief 从休眠队列中删除
 *
 * @param th
 */
thread_wait_entry_t *thread_sleep_del(thread_t *th)
{
    assert(cpulock_get_status());
    thread_wait_entry_t *pos = NULL;

    slist_foreach_not_next(
        pos, (slist_head_t *)&wait_list,
        node)
    {
        assert(pos->th->status == THREAD_SUSPEND);
        thread_wait_entry_t *next = slist_next_entry(
            pos, (slist_head_t *)wait_list,
            node);
        if (pos->th == th)
        {
            slist_del(&pos->node);
            break;
        }
        pos = next;
    }
    return pos;
}
void thread_sleep_del_and_wakeup(thread_t *th)
{
    assert(cpulock_get_status());
    thread_wait_entry_t *wait;

    wait = thread_sleep_del(th);
    if (wait == NULL)
    {
        return;
    }
    thread_ready(wait->th, TRUE);
}
/**
 * @brief 放到休眠队列中
 *
 * @param tick
 * @return umword_t 返回剩余的休眠时间
 */
umword_t thread_sleep(umword_t tick)
{
    assert(cpulock_get_status());

    thread_wait_entry_t entry;
    thread_t *cur_th = thread_get_current();
    umword_t status;

    thread_wait_entry_init(&entry, cur_th, tick);
    status = spinlock_lock(&lock);
    slist_add(&wait_list, &entry.node);
    thread_suspend(cur_th);
    spinlock_set(&lock, status);
    preemption();

    return entry.times;
}