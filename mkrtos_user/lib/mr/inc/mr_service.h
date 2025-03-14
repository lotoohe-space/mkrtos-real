/*
 * @copyright (c) 2023, MR Development Team
 *
 * @license SPDX-License-Identifier: Apache-2.0
 *
 * @date 2023-10-20    MacRsh       First version
 */

#ifndef _MR_SERVICE_H_
#define _MR_SERVICE_H_

#include "mr_def.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief This macro function asserts a condition.
 *
 * @param ex The condition to assert.
 */
#define mr_assert(ex)                   \
    do{                                 \
        if (!(ex))                      \
        {                               \
            mr_printf("assert > "       \
                      "failed: %s, "    \
                      "file: %s, "      \
                      "line: %d.\r\n",  \
                      #ex,              \
                      (__FUNCTION__),   \
                      (__LINE__));      \
            while(1);                   \
        }                               \
    } while(0)

/**
 * @brief This macro function logs a message.
 *
 * @param fmt The format of the message.
 * @param ... The arguments of the format.
 */
#define mr_log(fmt, ...)                \
    do{                                 \
        mr_printf("log > "              \
                  fmt".\r\n",           \
                  ##__VA_ARGS__);       \
    } while(0)

/**
 * @brief This macro function gets its structure from its member.
 *
 * @param pointer The pointer to the structure.
 * @param type The type of the structure.
 * @param member The member of the structure.
 *
 * @return A pointer to the structure.
 */
#define mr_container_of(pointer, type, member) \
    ((type *)((char *)(pointer) - (unsigned long)(&((type *)0)->member)))

/**
 * @brief This macro function checks if a value is set.
 *
 * @param value The value to check.
 * @param mask The mask to check.
 */
#define mr_bits_is_set(value, mask)     (((value) & (mask)) == (mask))

/**
 * @brief This macro function sets a value.
 *
 * @param value The value to set.
 * @param mask The mask to set.
 */
#define mr_bits_set(value, mask)        ((value) |= (mask))

/**
 * @brief This macro function clears a value.
 *
 * @param value The value to clear.
 * @param mask The mask to clear.
 */
#define mr_bits_clr(value, mask)        ((value) &= ~(mask))

/**
 * @brief This macro function gets the number of elements in an array.
 *
 * @param array The array.
 *
 * @return The number of elements in the array.
 */
#define mr_array_num(array)             (sizeof(array)/sizeof((array)[0]))

/**
 * @brief This macro function creates a local variable.
 *
 * @param type The type of the variable.
 * @param value The value of the variable.
 *
 * @return A pointer to the variable.
 */
#define mr_make_local(type, value)      (&((type){(value)}))

/**
 * @brief This macro function checks if a list is empty.
 *
 * @param list The list to check.
 *
 * @return True if the list is empty, mr_false otherwise.
 */
#define mr_list_is_empty(list)          (((list)->next) == (list))

/**
* @brief This function initialize a double list.
*
* @param list The list to initialize.
*/
MR_INLINE void mr_list_init(struct mr_list *list)
{
    list->next = list;
    list->prev = list;
}

/**
 * @brief This function insert a node after a node in a double list.
 *
 * @param list The list to insert after.
 * @param node The node to insert.
 */
MR_INLINE void mr_list_insert_after(struct mr_list *list, struct mr_list *node)
{
    list->next->prev = node;
    node->next = list->next;
    list->next = node;
    node->prev = list;
}

/**
 * @brief This function insert a node before a node in a double list.
 *
 * @param list The list to insert before.
 * @param node The node to insert.
 */
MR_INLINE void mr_list_insert_before(struct mr_list *list, struct mr_list *node)
{
    list->prev->next = node;
    node->prev = list->prev;
    list->prev = node;
    node->next = list;
}

/**
 * @brief This function remove a node from a double list.
 *
 * @param node The node to remove.
 */
MR_INLINE void mr_list_remove(struct mr_list *node)
{
    node->next->prev = node->prev;
    node->prev->next = node->next;
    node->next = node->prev = node;
}

/**
 * @brief This function get the length of a double list.
 *
 * @param list The list to get the length of.
 *
 * @return The length of the list.
 */
MR_INLINE size_t mr_list_get_length(struct mr_list *list)
{
    size_t length = 0;
    struct mr_list *node = list;

    while (node->next != list)
    {
        node = node->next;
        length++;
    }

    return length;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MR_SERVICE_H_ */
