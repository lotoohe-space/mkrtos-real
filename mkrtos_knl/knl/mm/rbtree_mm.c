#include "mln_rbtree.h"
#include <assert.h>
#include <slab.h>
#include <util.h>
#include <init.h>
#include <arch.h>
#include <string.h>
static slab_t *rbtree_node_slab;

static void *mk_rbtree_pool_alloc_handler(void *pool, mln_size_t size)
{
    void *mem = NULL;

    assert(size < PAGE_SIZE);

    switch (size)
    {
    case sizeof(mln_rbtree_node_t):
        mem = slab_alloc(rbtree_node_slab);
        if (!mem)
        {
            return NULL;
        }
        memset(mem, 0, sizeof(mln_rbtree_node_t) + sizeof(void *));
        *((size_t *)mem) = size;
        mem = (char *)mem + sizeof(void *);
        break;
    default:
        assert(0);
        break;
    }
    return mem;
}
static void mk_rbtree_pool_free_handler(void *data)
{
    void *mem = (char *)data - sizeof(void *);
    size_t size = *((size_t *)mem);
    switch (size)
    {
    case sizeof(mln_rbtree_node_t):
        slab_free(rbtree_node_slab, mem);
        break;
    default:
        assert(0);
        break;
    }
}

static void rbtree_node_slab_init(void)
{
    rbtree_node_slab = slab_create(sizeof(mln_rbtree_node_t) + sizeof(void *),
                                   "rbtree node slab");
    assert(rbtree_node_slab);
}
INIT_KOBJ_MEM(rbtree_node_slab_init);

void rbtree_mm_init(mln_rbtree_t *root_rbtree)
{
    struct mln_rbtree_attr rbattr;

    rbattr.pool_alloc = mk_rbtree_pool_alloc_handler;
    rbattr.pool_free = mk_rbtree_pool_free_handler;
    rbattr.cmp = NULL;
    rbattr.data_free = NULL;
    mln_rbtree_init(&rbattr, root_rbtree);
}