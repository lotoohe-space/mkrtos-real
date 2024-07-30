/**
 * @file spinlock.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-09-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "types.h"
#include "cpulock.h"
#include "spinlock.h"
#include <arch.h>
#include <util.h>
#if IS_ENABLED(CONFIG_SMP)
#include <spinlock_arch.h>
#endif
#include <atomics.h>
void spinlock_init(spinlock_t *lock)
{
    lock->val &= ~3UL;
}
void spinlock_invalidate(spinlock_t *lock)
{
    atomic_or(&lock->val, 1UL);
}
bool_t spinlock_is_invalidation(spinlock_t *lock)
{
    return lock->val & 1UL;
}
mword_t spinlock_status(spinlock_t *lock)
{
    return !!(lock->val & 0x2UL);
}
mword_t spinlock_try_lock(spinlock_t *lock)
{
    umword_t status = 0;

    status = cpulock_lock();
    if (spinlock_is_invalidation(lock))
    {
        cpulock_set(status);
        return -1;
    }
#if IS_ENABLED(CONFIG_SMP)
    status = spinlock_try_lock_arch(lock);
    _dmb(ish);
#endif
    return status;
}
mword_t spinlock_lock(spinlock_t *lock)
{
    umword_t status = 0;

    status = cpulock_lock();
    if (spinlock_is_invalidation(lock))
    {
        cpulock_set(status);
        return -1;
    }
#if IS_ENABLED(CONFIG_SMP)
    spinlock_lock_arch(lock);
    _dmb(ish);
#endif
    return status;
}
void spinlock_set(spinlock_t *lock, mword_t status)
{
#if IS_ENABLED(CONFIG_SMP)
    _dmb(ish);
    spinlock_unlock_arch(lock);
#endif
    cpulock_set(status);
}
