
#include <types.h>
#include <slist.h>
#include <dlist.h>
#include <buddy.h>
#include <slab.h>
#include <asm/mm.h>
#include <asm_config.h>
#include <printk.h>
#include <assert.h>
slab_t *slab_create(size_t align_size, const char *name)
{
    // assert(is_order(align_size));
    align_size = ALIGN(align_size, MWORD_BYTES);

    size_t need_size = SLAB_MIN_NUM * (align_size + sizeof(slab_block_head_t)) + sizeof(slab_t);
    size_t alloc_size = 1 << (is_power_of_2(need_size) ? ffs(need_size) : ffs(need_size) + 1);

    if (alloc_size < PAGE_SIZE)
    {
        alloc_size = PAGE_SIZE;
    }

    void *mem = buddy_alloc(buddy_get_alloter(), alloc_size);

    if (!mem)
    {
        return NULL;
    }
    printk("alloc addr 0x%x.\n", mem);
    slab_t *tmp_slab = (slab_t *)mem;

    spinlock_init(&tmp_slab->lock);
    slist_init(&tmp_slab->free_block);
    slist_init(&tmp_slab->used_block);
    slist_init(&tmp_slab->head);
    tmp_slab->free_nr = 0;
    tmp_slab->align_size = align_size;
    tmp_slab->total_nr = 0;

    for (size_t i = 0; i < alloc_size / (align_size + sizeof(slab_block_head_t)); i++)
    {
        if ((i + 1) * (align_size + sizeof(slab_block_head_t)) + (align_size + sizeof(slab_block_head_t)) > alloc_size)
        {
            break;
        }
        slab_block_head_t *slab_block = (slab_block_head_t *)((mem + sizeof(slab_t)) +
                                                              (i * (align_size + sizeof(slab_block_head_t))));

        slab_block->used = 0;
        slab_block->magic = SLAB_BLOCK_MAGIC;
        slist_init(&slab_block->next);

        slist_add(&tmp_slab->free_block, &slab_block->next);
        tmp_slab->free_nr++;
        tmp_slab->total_nr++;
    }

    return tmp_slab;
}
void slab_release(slab_t *slab)
{
    slab_head_t *slab_head_tmp;

    slist_foreach(slab_head_tmp, &slab->head, next)
    {
        buddy_free(buddy_get_alloter(), slab_head_tmp->buddy_addr);
    }
    buddy_free(buddy_get_alloter(), slab);
}

void *slab_alloc(slab_t *slab)
{
    assert(slab);
    mword_t status = spinlock_lock(&slab->lock);

again:
    if (slab->free_nr > 0)
    { // 有空闲地则直接分配
        slist_head_t *first = slist_first(&slab->free_block);

        slist_del(first);
        slist_add(&slab->used_block, first);
        slab_block_head_t *slab_block_free = container_of(first, slab_block_head_t, next);
        assert(slab_block_free->magic == SLAB_BLOCK_MAGIC);
        slab_block_free->used = 1;

        slab->free_nr--;
        spinlock_set(&slab->lock, status);
        return (void *)slab_block_free->user_data;
    }
    else
    {
        size_t need_size = SLAB_MIN_NUM * (slab->align_size + sizeof(slab_block_head_t)) + sizeof(slab_head_t);
        size_t alloc_size = 1 << (is_power_of_2(need_size) ? ffs(need_size) : ffs(need_size) + 1);
        if (alloc_size < PAGE_SIZE)
        {
            alloc_size = PAGE_SIZE;
        }
        void *mem = buddy_alloc(buddy_get_alloter(), alloc_size);

        if (!mem)
        {
            spinlock_set(&slab->lock, status);
            return NULL;
        }
        slab_head_t *slab_head = (slab_head_t *)mem;

        slab_head->buddy_addr = mem;
        // slab_head->size = alloc_size;
        slist_init(&slab_head->next);
        slist_add(&slab->head, &slab_head->next);

        for (size_t i = 0; i < alloc_size / (slab->align_size + sizeof(slab_block_head_t)); i++)
        {
            slab_block_head_t *slab_block = (slab_block_head_t *)((mem + sizeof(slab_head_t)) +
                                                                  (i * (slab->align_size + sizeof(slab_block_head_t))));

            slab_block->used = 0;
            slab_block->magic = SLAB_BLOCK_MAGIC;
            slist_init(&slab_block->next);

            slist_add(&slab->free_block, &slab_block->next);
            slab->free_nr++;
            slab->total_nr++;
        }
        goto again;
    }
    spinlock_set(&slab->lock, status);
}

void slab_free(slab_t *slab, void *mem)
{
    mword_t status = spinlock_lock(&slab->lock);

    slab_block_head_t *slab_block = container_of(mem, slab_block_head_t, user_data);

    assert(slab_block->magic == SLAB_BLOCK_MAGIC);
    assert(slab_block->used);
    slab_block->used = 0;
    slist_del(&slab_block->next);
    slist_add(&slab->free_block, &slab_block->next);

    slab->free_nr++;
    spinlock_set(&slab->lock, status);
}

void slab_print(slab_t *slab)
{
    printk("align size 0x%d, free nr %d, total nr %d\n", slab->align_size, slab->free_nr, slab->total_nr);
}
