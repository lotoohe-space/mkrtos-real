#pragma once

#include "types.h"

typedef struct ram_limit
{
    size_t max;
    size_t cur;
} ram_limit_t;

void ram_limit_init(ram_limit_t *limit, size_t max);
bool_t ram_limit_alloc(ram_limit_t *limit, size_t size);
void ram_limit_free(ram_limit_t *limit, size_t size);
