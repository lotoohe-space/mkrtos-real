#pragma once

#include "ram_limit.h"
#include <util.h>
void *mm_limit_alloc(ram_limit_t *limit, size_t size);
void mm_limit_free(ram_limit_t *limit, void *mem);
struct mem_heap *mm_get_free(struct mem_heap *next,
                             umword_t hope_size, umword_t *ret_addr);
void mm_trace(void);
void *mm_limit_alloc_align(ram_limit_t *limit, size_t size, size_t align);
void mm_limit_free_align(ram_limit_t *limit, void *mem, size_t size);
void mm_info(size_t *total, size_t *free);

#if IS_ENABLED(CONFIG_BUDDY_SLAB)
#include <buddy.h>
void *mm_limit_alloc_buddy(ram_limit_t *limit, size_t size);
void mm_limit_free_buddy(ram_limit_t *limit, void *mem, size_t size);
#include <slab.h>
void *mm_limit_alloc_slab(slab_t *slab, ram_limit_t *limit);
void mm_limit_free_slab(slab_t *slab, ram_limit_t *limit, void *mem);
#endif
