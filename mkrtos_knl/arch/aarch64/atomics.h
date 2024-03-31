#pragma once
#include <types.h>
typedef struct atomic64
{
    umword_t counter;
} atomic64_t;

typedef atomic64_t atomic_t;
#define ATOMIC64_INIT(i) ((atomic64_t){(i)})

#define atomic_read(v) (*(volatile umword_t *)(&(v)->counter))
#define atomic_set(v, i) (((v)->counter) = i)

bool_t atomic_cmpxchg(umword_t *v, umword_t old, umword_t new);
void atomic_and(umword_t *l, umword_t val);
umword_t atomic_and_return(umword_t *l, umword_t val);
void atomic_or(umword_t *l, umword_t val);
umword_t atomic_sub_return(umword_t i, atomic64_t *v);
umword_t atomic_add_return(umword_t i, atomic64_t *v);
umword_t atomic_fetch_and(umword_t i, atomic64_t *v);
umword_t atomic_fetch_or(umword_t i, atomic64_t *v);

static inline void atomic_add(umword_t i, atomic64_t *v)
{
    atomic_add_return(i, v);
}
static inline void atomic_sub(umword_t i, atomic64_t *v)
{
    atomic_sub_return(i, v);
}
static inline void atomic_inc(atomic64_t *v)
{
    atomic_add_return(1, v);
}
static inline void atomic_dec(atomic64_t *v)
{
    atomic_sub_return(1, v);
}
static inline umword_t atomic_inc_return(atomic64_t *v)
{
    return atomic_add_return(1, v);
}
static inline umword_t atomic_dec_return(atomic64_t *v)
{
    return atomic_sub_return(1, v);
}

static inline umword_t atomic_add_and_test(umword_t i, atomic64_t *v)
{
    return atomic_add_return(i, v) == 0;
}

static inline umword_t atomic_sub_and_test(umword_t i, atomic64_t *v)
{
    return atomic_sub_return(i, v) == 0;
}
static inline umword_t atomic_inc_and_test(atomic64_t *v)
{
    return atomic_inc_return(v) == 0;
}
static inline umword_t atomic_dec_and_test(atomic64_t *v)
{
    return atomic_dec_return(v) == 0;
}
static inline umword_t atomic_add_negative(umword_t i, atomic64_t *v)
{
    return (umword_t)atomic_add_return(i, v) < 0;
}
