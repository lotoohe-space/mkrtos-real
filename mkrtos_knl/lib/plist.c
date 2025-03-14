// /*
//  * lib/plist.c
//  * Based on simple lists (include/linux/list.h).
//  *
//  * This file contains the add / del functions which are considered to
//  * be too large to inline. See include/linux/plist.h for further
//  * information.
//  */

// #include "plist.h"


// #define plist_check_head(h)    do { } while (0)


// /**
//  * plist_add - add @node to @head
//  *
//  * @node:    &struct plist_node pointer
//  * @head:    &struct plist_head pointer
//  */
// void plist_add(struct plist_node *node, struct plist_head *head)
// {
//     struct plist_node *first, *iter, *prev = NULL;
//     struct list_head *node_next = &head->node_list;

//     plist_check_head(head);
//     WARN_ON(!plist_node_empty(node));
//     WARN_ON(!slist_is_empty(&node->prio_list));

//     if (plist_head_empty(head))
//         goto ins_node;

//     first = iter = plist_first(head);

//     do {
//         if (node->prio < iter->prio) {
//             node_next = &iter->node_list;
//             break;
//         }

//         prev = iter;
//         iter = slist_entry(iter->prio_list.next, struct plist_node, prio_list);
//     } while (iter != first);

//     if (!prev || prev->prio != node->prio)
//         slist_add_append(&iter->prio_list, &node->prio_list);
// ins_node:
//     slist_add_append(node_next, &node->node_list);

//     plist_check_head(head);
// }

// /**
//  * plist_del - Remove a @node from plist.
//  *
//  * @node:    &struct plist_node pointer - entry to be removed
//  * @head:    &struct plist_head pointer - list head
//  */
// void plist_del(struct plist_node *node, struct plist_head *head)
// {
//     plist_check_head(head);

//     if (!slist_is_empty(&node->prio_list)) {
//         if (node->node_list.next != &head->node_list) {
//             struct plist_node *next;

//             next = slist_entry(node->node_list.next, struct plist_node, node_list);

//             /* add the next plist_node into prio_list */
//             if (slist_is_empty(&next->prio_list))
//                 slist_add(&node->prio_list, &next->prio_list);
//         }
//         list_del_init(&node->prio_list);
//     }

//     list_del_init(&node->node_list);

//     plist_check_head(head);
// }

// /**
//  * plist_requeue - Requeue @node at end of same-prio entries.
//  *
//  * This is essentially an optimized plist_del() followed by
//  * plist_add().  It moves an entry already in the plist to
//  * after any other same-priority entries.
//  *
//  * @node:    &struct plist_node pointer - entry to be moved
//  * @head:    &struct plist_head pointer - list head
//  */
// void plist_requeue(struct plist_node *node, struct plist_head *head)
// {
//     struct plist_node *iter;
//     struct list_head *node_next = &head->node_list;

//     plist_check_head(head);
//     BUG_ON(plist_head_empty(head));
//     BUG_ON(plist_node_empty(node));

//     if (node == plist_last(head))
//         return;

//     iter = plist_next(node);

//     if (node->prio != iter->prio)
//         return;

//     plist_del(node, head);

//     plist_for_each_continue(iter, head) {
//         if (node->prio != iter->prio) {
//             node_next = &iter->node_list;
//             break;
//         }
//     }
//     slist_add_append(node_next, &node->node_list);

//     plist_check_head(head);
// }