
#include <types.h>
#include <spinlock.h>

void spinlock_lock_arch(spinlock_t *lock)
{
    mword_t dummy;
    mword_t tmp;

    asm volatile(
        "sevl\n"
        "prfm pstl1keep, [%[lock]]\n"
        "1: wfe\n"
        "ldaxr %x[dummy],[%[lock]]\n"
        "cbnz %x[dummy],1b\n"
        "orr %x[tmp],%x[dummy],#1\n"
        "stxr %w[dummy], %x[tmp], [%[lock]]\n"
        "cbnz %w[dummy],1b\n"
        : [dummy] "=&r"(dummy), [tmp] "=&r"(tmp), "+m"(lock->val)
        : [lock] "r"(&lock->val)
        : "cc");
}
void spinlock_unlock_arch(spinlock_t *lock)
{
    asm volatile(
        "stlr wzr, %[lock]"
        : [lock] "=Q"(lock->val));
}
