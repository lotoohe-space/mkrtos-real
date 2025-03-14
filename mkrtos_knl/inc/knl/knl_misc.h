#pragma once

#include "mm_space.h"
#include "ram_limit.h"
#include "types.h"

void *mpu_ram_alloc(mm_space_t *ms, ram_limit_t *r_limit, size_t ram_size, int mem_block);
