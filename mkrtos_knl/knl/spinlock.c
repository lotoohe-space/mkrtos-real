#include "types.h"
#include "cpulock.h"
#include "spinlock.h"
void spinlock_init(spinlock_t *lock)
{
    lock->val &= ~3UL;
}
umword_t spinlock_lock(spinlock_t *lock)
{
    umword_t status = 0;

#if SMP
    status = cpulock_lock();
#else
#error “不支持SMP”
#endif
    return status;
}
void spinlock_set(spinlock_t *lock, int status)
{
#if SMP
    cpulock_set(status);
#else
#error “不支持SMP”
#endif
}
