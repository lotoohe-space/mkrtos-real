#pragma once

#include "cpulock.h"
#include "types.h"
#include "kobject.h"
#include <atomics.h>
typedef struct ref_counter
{
    atomic_t val;
} ref_counter_t;

static inline void ref_counter_init(ref_counter_t *ref)
{
    atomic_set(&ref->val, 0);
}
static inline void ref_counter_inc(ref_counter_t *ref)
{
    atomic_inc(&ref->val);
}
static inline int ref_counter_dec(ref_counter_t *ref)
{
    umword_t old_val = atomic_read(&ref->val);
    atomic_dec_return(&ref->val);
    return old_val;
}
static inline int ref_counter_val(ref_counter_t *ref)
{
    return atomic_read(&ref->val);
}

int ref_counter_dec_and_release(ref_counter_t *ref, kobject_t *kobj);
