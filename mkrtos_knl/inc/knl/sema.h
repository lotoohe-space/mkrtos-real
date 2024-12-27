#pragma once

#include <spinlock.h>
#include <slist.h>

typedef struct sema
{
    kobject_t kobj;            //!< 内核对象节点
    spinlock_t lock;           //!<
    int cnt;                   //!< 计数
    int max_cnt;               //!< 最大计数
    slist_head_t suspend_head; //!< 挂起的线程
} sema_t;

typedef sema_t mutex_t;

void sema_init(sema_t *obj, int cnt, int max);
void sema_up(sema_t *obj);
void sema_down(sema_t *obj);

static inline void mutex_init(mutex_t *lock)
{
    sema_init(lock, 1, 1);
}
static inline void mutex_lock(mutex_t *lock)
{
    sema_down(lock);
}
static inline void mutex_unlock(mutex_t *lock)
{
    sema_up(lock);
}
