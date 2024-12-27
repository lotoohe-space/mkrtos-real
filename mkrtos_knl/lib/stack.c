
#include "types.h"
#include <assert.h>
#include <string.h>
#include "stack.h"

void stack_init(stack_t *stack, void *stack_data, int len, int item_size)
{
    assert(stack);
    assert(stack_data);
    memset(stack_data, 0, len * item_size);
    stack->top = -1;
    stack->len = len;
    stack->item_size = item_size;
    stack->data = stack_data;
}
int stack_push(stack_t *stack, void *item)
{
    if (stack->top < stack->len)
    {
        ++(stack->top);
        memcpy((uint8_t *)(stack->data) + stack->top * stack->item_size,
               item, stack->item_size);
        return 0;
    }
    return -1;
}
int stack_pop(stack_t *stack, void *item)
{
    if (stack->top != -1)
    {
        memcpy(item, (uint8_t *)(stack->data) + stack->top * stack->item_size, stack->item_size);
        stack->top--;
        return 0;
    }
    return -1;
}
