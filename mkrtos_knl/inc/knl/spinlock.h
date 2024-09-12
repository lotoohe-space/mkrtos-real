#pragma once

#include "types.h"

typedef struct spinlock
{
    umword_t val;
} spinlock_t;

void spinlock_init(spinlock_t *lock);
void spinlock_invalidate(spinlock_t *lock);
bool_t spinlock_is_invalidation(spinlock_t *lock);
mword_t spinlock_lock(spinlock_t *lock);
mword_t spinlock_try_lock(spinlock_t *lock);
void spinlock_set(spinlock_t *lock, mword_t status);
mword_t spinlock_status(spinlock_t *lock);
