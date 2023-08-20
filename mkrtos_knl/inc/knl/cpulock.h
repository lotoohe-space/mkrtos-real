#pragma once
#include "types.h"

umword_t cpulock_lock(void);
umword_t cpulock_get_status(void);
void cpulock_set(umword_t s);