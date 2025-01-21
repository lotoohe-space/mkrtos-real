#pragma once

typedef struct
{
    void *data;
    int top;
    int len;
    int item_size;
} stack_t;

void stack_init(stack_t *stack, void *stack_data, int len, int item_size);
int stack_len(stack_t *stack);
int stack_push(stack_t *stack, void *item);
int stack_pop(stack_t *stack, void *item);
