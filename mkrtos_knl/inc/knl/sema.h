#pragma once

#include <spinlock.h>
#include <slist.h>
#include <kobject.h>

typedef struct sema
{
    kobject_t kobj;            //!< 内核对象节点
    spinlock_t lock;           //!<
    int cnt;                   //!< 计数
    int max_cnt;               //!< 最大计数
    kobject_t *hold_th;        //!< 那个线程获取到了信号量，只有当max_cnt为1时才能有效，因为>1时可能有多个消费者
    int hold_th_prio;          //!< 获取mutex线程的优先级.
    slist_head_t suspend_head; //!< 挂起的线程FIXME:修改为plist
} sema_t;

typedef sema_t mutex_t;

void sema_init(sema_t *obj, int cnt, int max);
void sema_up(sema_t *obj);
umword_t sema_down(sema_t *obj, umword_t ticks);

static inline void mutex_init(mutex_t *lock)
{
    sema_init(lock, 1, 1);
}
static inline void mutex_lock(mutex_t *lock, umword_t ticks)
{
    sema_down(lock, ticks);
}
static inline void mutex_unlock(mutex_t *lock)
{
    sema_up(lock);
}
