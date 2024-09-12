
#include <types.h>
#include <slist.h>
#include <util.h>
#include <arch.h>
#include <buddy.h>
#include <asm/mm.h>
#include <printk.h>
#include <assert.h>
#include <spinlock.h>
#include <string.h>
static buddy_entry_t *buddy_entry_simp_slab;
static size_t buddy_entry_cn = 0;
static buddy_order_t buddy_kmem;

static inline size_t to_size(size_t order);

static inline void buddy_entry_set_use_state(buddy_entry_t *be, bool_t state)
{
    be->addr &= ~0x2ULL;
    be->addr |= (!!state) << 1;
}
static inline int buddy_entry_get_use_state(buddy_entry_t *be)
{
    return !!(be->addr & 0x2ULL);
}
static inline void buddy_entry_set_valid_state(buddy_entry_t *be, bool_t state)
{
    be->addr &= ~0x1ULL;
    be->addr |= (!!state) << 0;
}
static inline void buddy_entry_set_addr(buddy_entry_t *be, addr_t addr)
{
    be->addr = GET_LSB(be->addr, 2) | MASK_LSB(addr, 2);
}
static inline void buddy_entry_init(buddy_entry_t *be, addr_t addr)
{
    be->addr &= ~(BUDDY_ENTRY_ATTR_MASK);
    be->addr |= 1 | addr;
}
static inline void buddy_entry_set_lr(buddy_entry_t *be, bool_t l)
{
    be->addr &= ~0xCULL;
    be->addr |= ((l ? 0x3 : 0x1) << 2ULL);
}
static inline void buddy_entry_clr_lr(buddy_entry_t *be)
{
    be->addr &= ~0xCULL;
}

static buddy_entry_t *buddy_entry_alloc(void)
{
    for (int i = 0; i < buddy_entry_cn; i++)
    {
        if (!BUDDY_ENTRY_VALID(&buddy_entry_simp_slab[i]))
        {
            buddy_entry_simp_slab[i].addr = 0;
            buddy_entry_simp_slab[i].parent = NULL;
            buddy_entry_set_valid_state(&buddy_entry_simp_slab[i], TRUE);
            // printk("allod buddy entry %d.\n", i);
            return &buddy_entry_simp_slab[i];
        }
    }
    assert(0);
    return NULL;
}
static void buddy_entry_free(buddy_entry_t *be)
{
    be->addr &= ~(BUDDY_ENTRY_ATTR_MASK);
}

buddy_order_t *buddy_get_alloter(void)
{
    return &buddy_kmem;
}
#define BUDDY_MAX_BYTES (1 << (CONFIG_PAGE_SHIFT + BUDDY_MAX_ORDER))
int buddy_init(buddy_order_t *buddy, addr_t start_addr, size_t size)
{
    assert(buddy);

    if (start_addr % BUDDY_MAX_BYTES)
    {
        return -1;
    }
    if (size % BUDDY_MAX_BYTES)
    {
        return -1;
    }
    spinlock_init(&buddy->lock);
    for (int i = 0; i < BUDDY_MAX_ORDER; i++)
    {
        slist_init(&buddy->order_tab[i].b_order);
    }
    size_t entry_cn = (size / BUDDY_MAX_BYTES) << (BUDDY_MAX_ORDER - 1);

    buddy_entry_simp_slab = (void *)start_addr;
    size_t entrys_size = ALIGN(entry_cn * (sizeof(buddy_entry_t)), BUDDY_MAX_BYTES);
    start_addr += entrys_size;
    size -= entrys_size;
    buddy->heap_addr = start_addr;
    buddy_entry_cn = entry_cn;

    printk("buddy start addr:[0x%lx - 0x%lx], buddy mem size:%dMB\n", start_addr,
           start_addr + size - 1, size / 1024 / 1024);

    size_t remain_size = size;
    addr_t add_addr = buddy->heap_addr;

    // 内存分布到不同地order中=
    while (remain_size)
    {
        buddy_entry_t *new_be = buddy_entry_alloc();
        if (!new_be)
        {
            return -1;
        }
        buddy_entry_init(new_be, add_addr);
        slist_add(&buddy->order_tab[BUDDY_MAX_ORDER - 1].b_order, &new_be->next);
        buddy->order_tab[BUDDY_MAX_ORDER - 1].nr_free++;

        remain_size -= BUDDY_MAX_BYTES;
        add_addr += BUDDY_MAX_BYTES;
    }
    buddy->heap_size = size;
    return 0;
}

static buddy_entry_t *buddy_order_find_free(buddy_head_t *buddy_head)
{
    buddy_entry_t *be_tmp = NULL;

    slist_foreach(be_tmp, &buddy_head->b_order, next)
    {
        if (!BUDDY_ENTRY_USED(be_tmp))
        {
            // 没有被使用
            return be_tmp;
        }
    }
    return NULL;
}
static inline size_t to_size(size_t order)
{
    return (1 << order) << PAGE_SHIFT;
}
/**
 * 从buddy指定位置向下分裂到指定位置，并返回分裂后的块中的一块
 */
static buddy_entry_t *buddy_order_div(buddy_order_t *buddy, mword_t st_order)
{
    assert(st_order < BUDDY_MAX_ORDER);
    assert(st_order > 0);

    // 找到一个空的
    buddy_entry_t *free_b = buddy_order_find_free(&buddy->order_tab[st_order]);

    if (!free_b)
    {
        return NULL;
    }
    // 设置被使用
    buddy_entry_set_use_state(free_b, TRUE);
    // 可用数量-1
    buddy->order_tab[st_order].nr_free--;

    buddy_entry_t *new_l = buddy_entry_alloc();
    buddy_entry_t *new_r = buddy_entry_alloc();

    if (!new_l || !new_r)
    {
        if (new_l)
        {
            buddy_entry_free(new_l);
        }
        if (new_l)
        {
            buddy_entry_free(new_r);
        }
        buddy_entry_set_use_state(free_b, FALSE);
        buddy->order_tab[st_order].nr_free++;
        return NULL;
    }

    addr_t div_addr = BUDDY_ENTRY_ADDR(free_b);

    new_l->parent = free_b;
    new_r->parent = free_b;
    buddy_entry_init(new_l, div_addr);
    buddy_entry_init(new_r, div_addr + to_size(st_order - 1));
    buddy_entry_set_lr(new_l, TRUE);  // 设置为左边
    buddy_entry_set_lr(new_r, FALSE); // 设置为右边
    slist_add(&buddy->order_tab[st_order - 1].b_order, &new_l->next);
    slist_add(&buddy->order_tab[st_order - 1].b_order, &new_r->next);

    // printk("div: l %d, r %d,valid %d, used %d, addr 0x%x, block size 0x%x\n",
    //     BUDDY_ENTRY_IS_L(new_l),
    //     BUDDY_ENTRY_IS_R(new_l),
    //     BUDDY_ENTRY_VALID(new_l),
    //     BUDDY_ENTRY_USED(new_l),
    //     BUDDY_ENTRY_ADDR(new_l), to_size(st_order - 1));
    // printk("div: l %d, r %d,valid %d, used %d, addr 0x%x, block size 0x%x\n",
    //     BUDDY_ENTRY_IS_L(new_r),
    //     BUDDY_ENTRY_IS_R(new_r),
    //     BUDDY_ENTRY_VALID(new_r),
    //     BUDDY_ENTRY_USED(new_r),
    //     BUDDY_ENTRY_ADDR(new_r), to_size(st_order - 1));

    buddy->order_tab[st_order - 1].nr_free += 2;
    return new_l;
}
void *buddy_alloc(buddy_order_t *buddy, size_t size)
{
    size_t need_ffs;
    void *ret_mem = NULL;

    need_ffs = ffs(size);
    need_ffs += is_power_of_2(size) ? 0 : 1;

    if (need_ffs < PAGE_SHIFT)
    { // 至少4K
        need_ffs = PAGE_SHIFT;
    }
    need_ffs -= PAGE_SHIFT;

    if (need_ffs >= BUDDY_MAX_ORDER)
    {
        return NULL;
    }

    mword_t l_state = spinlock_lock(&buddy->lock);

    if (buddy->order_tab[need_ffs].nr_free > 0)
    {
        // 有空闲的直接分配
        buddy_entry_t *free_b = buddy_order_find_free(&buddy->order_tab[need_ffs]);

        assert(free_b);
        buddy->order_tab[need_ffs].nr_free--;
        buddy_entry_set_use_state(free_b, TRUE);
        ret_mem = (void *)BUDDY_ENTRY_ADDR(free_b);
        goto end;
    }
    for (size_t st_inx = need_ffs + 1; st_inx < BUDDY_MAX_ORDER; st_inx++)
    {
        if (st_inx >= BUDDY_MAX_ORDER)
        {
            // 超过了可分配的大小直接退出
            spinlock_set(&buddy->lock, l_state);
            return NULL;
        }
        if (buddy->order_tab[st_inx].nr_free > 0)
        {
            buddy_entry_t *div;

            do
            {
                div = buddy_order_div(buddy, st_inx); //!< 不断地划分大小，直到大小合适
                st_inx--;
                if (st_inx == need_ffs)
                {
                    break;
                }
                // buddy_print(buddy);
            } while (div);
            if (!div)
            {
                ret_mem = NULL;
                goto end;
            }
            buddy->order_tab[st_inx].nr_free--;
            buddy_entry_set_use_state(div, TRUE);
            ret_mem = (void *)BUDDY_ENTRY_ADDR(div);
            goto end;
        }
    }
end:
    spinlock_set(&buddy->lock, l_state);
    // printk("alloc addr 0x%x.\n", ret_mem);
    return ret_mem;
}
static inline addr_t get_buddy_addr(buddy_entry_t *merge_be, size_t size)
{
    assert(is_power_of_2(size));

    if (BUDDY_ENTRY_IS_L(merge_be))
    {
        return BUDDY_ENTRY_ADDR(merge_be) + size;
    }
    else if (BUDDY_ENTRY_IS_R(merge_be))
    {
        return BUDDY_ENTRY_ADDR(merge_be) - size;
    }
    else
    {
        assert(0);
    }
}
static inline addr_t get_buddy_start(buddy_entry_t *merge_be, size_t size)
{
    assert(is_power_of_2(size));

    if (BUDDY_ENTRY_IS_L(merge_be))
    {
        return BUDDY_ENTRY_ADDR(merge_be);
    }
    else if (BUDDY_ENTRY_IS_R(merge_be))
    {
        return BUDDY_ENTRY_ADDR(merge_be) - size;
    }
    else
    {
        assert(0);
    }
}
static buddy_entry_t *buddy_merge(buddy_order_t *buddy, buddy_entry_t *merge_be, int st_ffs)
{
    if (st_ffs >= BUDDY_MAX_ORDER || merge_be->parent == NULL)
    {
        return NULL;
    }
    if (buddy_entry_get_use_state(merge_be))
    {
        return NULL;
    }
    addr_t buddy_addr = get_buddy_addr(merge_be, to_size(st_ffs));
    buddy_entry_t *tmp = NULL;

    slist_foreach(tmp, &buddy->order_tab[st_ffs].b_order, next)
    {
        if (BUDDY_ENTRY_ADDR(tmp) == (addr_t)buddy_addr)
        {
            if (BUDDY_ENTRY_USED(tmp))
            {
                return NULL;
            }
            assert(BUDDY_ENTRY_VALID(tmp));

            buddy_entry_t *merge_node;
            addr_t buddy_st_addr = get_buddy_start(tmp, to_size(st_ffs));

            slist_del(&merge_be->next);
            slist_del(&tmp->next);
            buddy->order_tab[st_ffs].nr_free -= 2;

            assert(tmp->parent);
            buddy_entry_set_use_state(tmp, FALSE);

            if (BUDDY_ENTRY_IS_L(merge_be))
            {
                merge_node = merge_be;
                buddy_entry_free(tmp);
            }
            else
            {
                merge_node = tmp;
                buddy_entry_free(merge_be);
            }
            buddy_entry_init(merge_node, buddy_st_addr);
            buddy->order_tab[st_ffs + 1].nr_free++;
            slist_add(&buddy->order_tab[st_ffs + 1].b_order, &merge_node->next);
            return tmp->parent;
        }
    }
    return NULL;
}
void buddy_free(buddy_order_t *buddy, void *mem)
{
    addr_t free_addr = (addr_t)mem;
    mword_t l_state = spinlock_lock(&buddy->lock);

    for (int i = 0; i < BUDDY_MAX_ORDER; i++)
    {
        buddy_entry_t *tmp = NULL;

        slist_foreach(tmp, &buddy->order_tab[i].b_order, next)
        {
            if (BUDDY_ENTRY_ADDR(tmp) == (addr_t)mem)
            {
                // assert(BUDDY_ENTRY_USED(tmp));
                assert(BUDDY_ENTRY_VALID(tmp));

                if (BUDDY_ENTRY_USED(tmp))
                {
                    buddy_entry_set_use_state(tmp, FALSE);
                    buddy->order_tab[i].nr_free++;
                }
                buddy_entry_t *next = tmp;

                do
                {
                    next = buddy_merge(buddy, next, i);
                    if (!next)
                    {
                        spinlock_set(&buddy->lock, l_state);
                        return;
                    }
                    i++;
                    // buddy_print(buddy);
                } while (1);
            }
        }
    }
    spinlock_set(&buddy->lock, l_state);
}

void buddy_print(buddy_order_t *buddy)
{
    // mword_t l_state = spinlock_test_and_lock(&buddy->lock);
    printk("\n");
    for (int i = 0; i < BUDDY_MAX_ORDER; i++)
    {
        buddy_entry_t *tmp = NULL;

        slist_foreach(tmp, &buddy->order_tab[i].b_order, next)
        {
            printk("div: l %d, r %d,valid %d, used %d, addr 0x%x, block size 0x%x\n",
                   BUDDY_ENTRY_IS_L(tmp),
                   BUDDY_ENTRY_IS_R(tmp),
                   BUDDY_ENTRY_VALID(tmp),
                   BUDDY_ENTRY_USED(tmp),
                   BUDDY_ENTRY_ADDR(tmp), to_size(i));
        }
    }
    // spinlock_set(&buddy->lock, l_state);
}
