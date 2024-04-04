#pragma once

#include <types.h>
#include <slist.h>
#include <spinlock.h>

#define SLAB_NAME_LEN 8

#define SLAB_BLOCK_MAGIC 0x1212343456567878

typedef struct slab_block_head
{
    slist_head_t next;
    mword_t used;
    mword_t magic;
    char user_data[0];
} slab_block_head_t;

typedef struct slab_head
{
    slist_head_t next;
    void *buddy_addr;
    // size_t size;
} slab_head_t;

#define SLAB_MIN_NUM 2 //!< 单次申请buddy内存时，保证里面至少有两块

// slab内存从buddy中分配
typedef struct slab
{
    slist_head_t head;
    // TODO: char name[SLAB_NAME_LEN];
    size_t align_size; //!< 每个slab项大小
    slist_head_t free_block;
    size_t free_nr;
    slist_head_t used_block;
    size_t total_nr;
    spinlock_t lock;
} slab_t;

static inline size_t slab_get_item_size(slab_t *slab)
{
    return slab->align_size;
}
slab_t *slab_create(size_t align_size, const char *name);
void slab_release(slab_t *slab);
void *slab_alloc(slab_t *slab);
void slab_free(slab_t *slab, void *mem);
void slab_print(slab_t *slab);
