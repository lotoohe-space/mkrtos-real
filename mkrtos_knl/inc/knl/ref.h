#pragma once

#include "cpulock.h"
#include "types.h"
#include "kobject.h"

typedef struct ref_counter
{
    int val;
} ref_counter_t;

static inline void ref_counter_init(ref_counter_t *ref)
{
    ref->val = 0;
}
static inline void ref_counter_inc(ref_counter_t *ref)
{
    /*TODO：原子变量*/
    umword_t status = cpulock_lock();
    ref->val++;
    cpulock_set(status);
}
static inline int ref_counter_dec(ref_counter_t *ref)
{
    int ret = 0;
    /*TODO：原子变量*/
    umword_t status = cpulock_lock();
    ret = ref->val;
    ref->val--;
    cpulock_set(status);
    return ret;
}
static inline int ref_counter_val(ref_counter_t *ref)
{
    return ref->val;
}

int ref_counter_dec_and_release(ref_counter_t *ref, kobject_t *kobj);
