#pragma once

#include <types.h>
#include <slist.h>
#include <util.h>
#include <spinlock.h>

typedef struct buddy_entry
{
    slist_head_t next;          //!< next
    addr_t addr;                //!< start addr
    struct buddy_entry *parent; //!< 从哪一个分出来
} buddy_entry_t;

#define BUDDY_ENTRY_ATTR_MASK 0xfULL
#define BUDDY_ENTRY_VALID(this) (!!((this)->addr & 0x1ULL))         //!< 1代表有效
#define BUDDY_ENTRY_USED(this) (!!((this)->addr & 0x2ULL))          //!< 1代表被分配了
#define BUDDY_ENTRY_IS_L(this) (!!(((this)->addr & 0xCULL) == 0xC)) //!< 伙伴地左边
#define BUDDY_ENTRY_IS_R(this) (!!(((this)->addr & 0xCULL) == 0x4))
#define BUDDY_ENTRY_ADDR(this) ((this)->addr & ~(BUDDY_ENTRY_ATTR_MASK)) //!< 获取使用的地址

typedef struct buddy_head
{
    slist_head_t b_order; //!< 链表的入口
    mword_t nr_free;      //!< 多少个可用的块
} buddy_head_t;

#define BUDDY_MAX_ORDER 12

typedef struct buddy_order
{
    buddy_head_t order_tab[BUDDY_MAX_ORDER]; // max 16MB
    addr_t heap_addr;
    spinlock_t lock;
    size_t heap_size;
} buddy_order_t;

buddy_order_t *buddy_get_alloter(void);
int buddy_init(buddy_order_t *buddy, addr_t start_addr, size_t size);
void *buddy_alloc(buddy_order_t *buddy, size_t size);
void buddy_free(buddy_order_t *buddy, void *mem);
static inline size_t buddy_get_heap_size(buddy_order_t *buddy)
{
    return buddy->heap_size;
}
static inline addr_t buddy_get_heap_addr(buddy_order_t *buddy)
{
    return buddy->heap_addr;
}
void buddy_print(buddy_order_t *buddy);
