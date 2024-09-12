
#include <spinlock.h>
#include <types.h>

mword_t spinlock_try_lock_arch(spinlock_t *lock)
{
    mword_t dummy = 0;
    mword_t tmp;

    asm volatile(
        "sevl\n"
        "prfm pstl1keep, [%[lock]]\n"
        // "1: wfe\n"
        "ldaxr %x[d],[%[lock]]\n"
        "tst %x[d],#2\n"
        "bne next\n"
        "orr %x[tmp],%x[d],#2\n"
        "stxr %w[d], %x[tmp], [%[lock]]\n"
        // "cbnz %w[d],next\n"
        "eor %w[d],%w[d],0x1\n"
        "next: nop\n"
        : [d] "=&r"(dummy), [tmp] "=&r"(tmp), "+m"(lock->val)
        : [lock] "r"(&lock->val)
        : "cc");

    return dummy;
}
void spinlock_lock_arch(spinlock_t *lock)
{
    mword_t dummy;
    mword_t tmp;

    asm volatile(
        "sevl\n"
        "prfm pstl1keep, [%[lock]]\n"
        "1: wfe\n"
        "ldaxr %x[d],[%[lock]]\n"
        "tst %x[d],#2\n"
        "bne 1b\n"
        "orr %x[tmp],%x[d],#2\n"
        "stxr %w[d], %x[tmp], [%[lock]]\n"
        "cbnz %w[d],1b\n"
        : [d] "=&r"(dummy), [tmp] "=&r"(tmp), "+m"(lock->val)
        : [lock] "r"(&lock->val)
        : "cc");
}
void spinlock_unlock_arch(spinlock_t *lock)
{
    umword_t tmp;

    asm volatile(
        "ldr %x[tmp], %[lock]\n"
        "bic %x[tmp], %x[tmp], #2\n"
        "stlr %x[tmp], %[lock]\n"
        : [lock] "=Q"(lock->val), [tmp] "=&r"(tmp));
}
