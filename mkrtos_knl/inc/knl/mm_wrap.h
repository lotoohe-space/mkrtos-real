#pragma once

#include "ram_limit.h"

void *mm_limit_alloc(ram_limit_t *limit, size_t size);
void mm_limit_free(ram_limit_t *limit, void *mem);
