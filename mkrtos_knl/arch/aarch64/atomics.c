
#include <types.h>
#include <atomics.h>

bool_t atomic_cmpxchg(umword_t *v, umword_t old, umword_t new)
{
    umword_t oldval, res;

    asm volatile(
        "1:\n"
        "ldxr %1, %2\n"
        "eor %0, %1, %3\n"
        "cbnz %0, 2f\n"
        "stxr %w0, %4, %2\n"
        "cbnz %w0, 1b\n"
        "2:"
        : "=&r"(res), "=&r"(oldval), "+Q"(*v)
        : "r"(old), "r"(new)
        : "cc");
    return !res;
}
void atomic_and(umword_t *l, umword_t val)
{
    umword_t tmp, ret;

    asm volatile(
        "1:\n"
        "ldxr %0, %2\n"
        "and %0, %0, %3\n"
        "stxr %w1, %0, %2\n"
        "cbnz %w1, 1b\n"
        : "=&r"(tmp), "=&r"(ret), "+Q"(*l)
        : "r"(val)
        : "cc");
}
umword_t atomic_and_return(umword_t *l, umword_t val)
{
    umword_t tmp, ret;

    asm volatile(
        "ldxr %0, %2\n"
        "and %1, %0, %3\n"
        : "=&r"(tmp), "=&r"(ret), "+Q"(*l)
        : "r"(val)
        : "cc");
    return !!ret;
}
void atomic_or(umword_t *l, umword_t val)
{
    umword_t tmp, ret;

    asm volatile(
        "1:\n"
        "ldxr %0, %2\n"
        "orr %0, %0, %3\n"
        "stxr %w1, %0, %2\n"
        "cbnz %w1, 1b\n"
        : "=&r"(tmp), "=&r"(ret), "+Q"(*l)
        : "r"(val)
        : "cc");
}
umword_t atomic_sub_return(umword_t i, atomic64_t *v)
{
    umword_t tmp;
    umword_t ret;

    asm volatile(
        "1:\n"
        "ldxr %0, %2\n"
        "sub %0, %0, %3\n"
        "stxr %w1, %0, %2\n"
        "cbnz %w1, 1b\n"
        : "=&r"(ret), "=&r"(tmp), "+Q"(v->counter)
        : "r"(i));
    return ret;
}
umword_t atomic_add_return(umword_t i, atomic64_t *v)
{
    umword_t tmp;
    umword_t ret;

    asm volatile(
        "1:\n"
        "ldxr %0, %2\n"
        "add %0, %0, %3\n"
        "stxr %w1, %0, %2\n"
        "cbnz %w1, 1b\n"
        : "=&r"(ret), "=&r"(tmp), "+Q"(v->counter)
        : "r"(i));
    return ret;
}
umword_t atomic_fetch_and(umword_t i, atomic64_t *v)
{
    umword_t tmp, ret;
    umword_t old;

    asm volatile(
        "1:\n"
        "ldxr %2, %3\n"
        "and %0, %2, %4\n"
        "stxr %w1, %0, %3\n"
        "cbnz %w1, 1b\n"
        : "=&r"(tmp), "=&r"(ret), "=&r"(old), "+Q"(v->counter)
        : "r"(i)
        : "cc");
    return old;
}
umword_t atomic_fetch_or(umword_t i, atomic64_t *v)
{
    umword_t tmp, ret;
    umword_t old;

    asm volatile(
        "1:\n"
        "ldxr %2, %3\n"
        "orr %0, %2, %4\n"
        "stxr %w1, %0, %3\n"
        "cbnz %w1, 1b\n"
        : "=&r"(tmp), "=&r"(ret), "=&r"(old), "+Q"(v->counter)
        : "r"(i)
        : "cc");
    return old;
}
