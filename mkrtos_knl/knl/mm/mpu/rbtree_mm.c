#include "mln_rbtree.h"
#include <assert.h>
#include <slab.h>
#include <util.h>
#include <init.h>
#include <arch.h>
#include <string.h>
#include "mm_wrap.h"

static void *mk_rbtree_pool_alloc_handler(void *pool, mln_size_t size)
{
    void *m;
    m = mm_alloc(size);

    return m;
}
static void mk_rbtree_pool_free_handler(void *data)
{
    mm_free(data);
}

void rbtree_mm_init(mln_rbtree_t *root_rbtree)
{
    struct mln_rbtree_attr rbattr;

    rbattr.pool_alloc = mk_rbtree_pool_alloc_handler;
    rbattr.pool_free = mk_rbtree_pool_free_handler;
    rbattr.cmp = NULL;
    rbattr.data_free = NULL;
    mln_rbtree_init(&rbattr, root_rbtree);
}