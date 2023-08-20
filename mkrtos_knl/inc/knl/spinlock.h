#pragma once

#include "types.h"

typedef struct spinlock
{
    umword_t val;
} spinlock_t;

void spinlock_init(spinlock_t *lock);
umword_t spinlock_lock(spinlock_t *lock);
void spinlock_set(spinlock_t *lock, int status);
