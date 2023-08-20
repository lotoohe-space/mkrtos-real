#pragma once

#include "types.h"

/**
 * @brief 单链表
 */
typedef struct dlist_item
{
    struct dlist_item *next; //!< 下一个
} dlist_item_t;

typedef struct dlist_head
{
    struct dlist_item *first; //!< 第一个
} dlist_head_t;

static inline void dlist_head_init(dlist_head_t *list)
{
    list->first = NULL;
}
static inline void dlist_item_init(dlist_item_t *item)
{
    item->next = NULL;
}
static inline void dlist_add(dlist_item_t *prev, dlist_item_t *item)
{
    item->next = prev->next;
    prev->next = item;
}
static inline void dlist_add_head(dlist_head_t *head, dlist_item_t *item)
{
    item->next = head->first;
    head->first = item;
}
static inline void dlist_add_tail(dlist_head_t *head, dlist_item_t *item)
{
    dlist_item_t *first = head->first;

    if (!first)
    {
        item->next = NULL;
        head->first = item;
        return;
    }
    while (first->next)
    {
        first = first->next;
    }
    item->next = NULL;
    first->next = item;
}
static inline void dlist_del_curr(dlist_item_t *prev, dlist_item_t *curr)
{
    prev->next = curr->next;
}

static inline void dlist_del(dlist_head_t *head, dlist_item_t *del)
{
    dlist_item_t *prev = head->first;
    dlist_item_t *curr = head->first;

    while (curr)
    {
        if (curr == del)
        {
            if (prev == head->first)
            {
                head->first = curr->next;
                return;
            }
            else
            {
                prev->next = curr->next;
                return;
            }
        }
        prev = curr;
        curr = curr->next;
    }
}

#define dlist_entry(ptr, type, member) container_of(ptr, type, member)

/**
 * @brief 循环获得每个节点的入口
 */
#define dlist_foreach(head, item, type, member)           \
    for (item = dlist_entry((head)->first, type, member); \
         &((item)->member) != NULL;                       \
         item = dlist_entry((item)->member.next, type, member))
