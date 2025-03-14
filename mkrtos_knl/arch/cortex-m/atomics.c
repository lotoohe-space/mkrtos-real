
#include <types.h>
#include <atomics.h>
#include <cpulock.h>
bool_t atomic_cmpxchg(umword_t *v, umword_t old, umword_t new)
{
    bool_t ret = FALSE;
    mword_t status = cpulock_lock();
    if (*v == old) {
        *v = new;
        ret = TRUE;
    }
    cpulock_set(status);
    return ret;
}
void atomic_and(umword_t *l, umword_t val)
{
    mword_t status = cpulock_lock();
    *l &= val;
    cpulock_set(status);
}
umword_t atomic_and_return(umword_t *l, umword_t val)
{
    umword_t ret;
    mword_t status = cpulock_lock();
    *l &= val;
    ret = !!(*l);
    cpulock_set(status);
    return ret;
}
void atomic_or(umword_t *l, umword_t val)
{
    mword_t status = cpulock_lock();
    *l |= val;
    cpulock_set(status);
}
umword_t atomic_sub_return(umword_t i, atomic_t *v)
{
    umword_t ret;
    mword_t status = cpulock_lock();
    v->counter -= i;
    ret = (v->counter);
    cpulock_set(status);
    return ret;
}
umword_t atomic_add_return(umword_t i, atomic_t *v)
{
    umword_t ret;
    mword_t status = cpulock_lock();
    v->counter += i;
    ret = (v->counter);
    cpulock_set(status);
    return ret;
}
umword_t atomic_fetch_and(umword_t i, atomic_t *v)
{
    umword_t ret;
    mword_t status = cpulock_lock();
    ret = (v->counter);
    v->counter &= i;
    cpulock_set(status);
    return ret;
}
umword_t atomic_fetch_or(umword_t i, atomic_t *v)
{
    umword_t ret;
    mword_t status = cpulock_lock();
    ret = (v->counter);
    v->counter |= i;
    cpulock_set(status);
    return ret;
}
