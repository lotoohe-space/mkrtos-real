#pragma once

#include <spinlock.h>

void spinlock_lock_arch(spinlock_t *lock);
void spinlock_unlock_arch(spinlock_t *lock);
