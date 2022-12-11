#ifndef _SLIST_H_
#define _SLIST_H_
#include <type.h>
#include <mkrtos.h>

/**
 * @brief 循环双向链表
 */
typedef struct slist_head {
    struct slist_head *prev; //!< 前一个
    struct slist_head *next; //!< 后一个
} slist_head_t;

static inline void slist_init(slist_head_t *list)
{
    list->prev = list;
    list->next = list;
}
/**
 * @brief is empty
 */
static inline int64_t slist_is_empty(slist_head_t *list)
{
    return (list->prev == list && list->next == list) ||
        (list->prev == 0 && list->next == 0);
}
static inline void slist_add(slist_head_t *head, slist_head_t *item)
{
    item->next = head->next;
    item->prev = head;
    head->next->prev = item;
    head->next = item;
}
// static inline void slist_add_append(slist_head_t *head, slist_head_t *item)
// {
//     slist_head_t *tail = head->prev;

//     return slist_add(head, tail);
// }
static inline void slist_del(slist_head_t *item)
{
    item->prev->next = item->next;
    item->next->prev = item->prev;
}

#define slist_entry(ptr, type, member) container_of(ptr, type ,member)

/**
 * @brief 遍历每一个节点获得struct的入口
 */
#define slist_foreach(pos, head, member) \
    for (pos = slist_entry((head)->next, typeof(*pos), member)\
        ; &(pos)->member != (head) && !slist_is_empty(head)\
        ;pos = slist_entry((pos)->member.next, typeof(*pos), member)\
    )

#endif
