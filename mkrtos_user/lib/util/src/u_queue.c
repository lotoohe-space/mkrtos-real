/**
 * @file u_queue.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-08-27
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "u_types.h"
#include "u_queue.h"
#include <assert.h>

void q_init(queue_t *q, uint8_t *data, int size)
{
    q->front = 0;
    q->rear = 0;
    q->m = data;
    q->size = size;
}
int q_empty(queue_t *q)
{
    assert(q);
    if (q->front == q->rear)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
int q_enqueue(queue_t *q, uint8_t e)
{
    if (((q->rear + 1) % q->size) == q->front)
    {
        return -1;
    }
    q->m[q->rear] = e;
    q->rear = (q->rear + 1) % q->size;
    return 0;
}
int q_dequeue(queue_t *q, uint8_t *e)
{
    if (q->front == q->rear)
    {
        return -1;
    }
    *e = q->m[q->front];
    q->front = (q->front + 1) % q->size;
    return 0;
}
int q_get_tail(queue_t *q, uint8_t *e)
{
    if (q->rear == q->front)
    { // 空的
        return -1;
    }
    if (e)
    {
        *e = q->m[q->rear];
    }
    return 0;
}
int q_dequeue_tail(queue_t *q, uint8_t *e)
{
    if (q->rear == q->front)
    { // 空的
        return -1;
    }
    q->rear = (q->rear - 1 + q->size) % q->size;
    if (e)
    {
        *e = q->m[q->rear];
    }
    return 0;
}
int q_queue_len(queue_t *q)
{
    return (q->rear - q->front + q->size) % q->size;
}
void q_queue_clear(queue_t *q)
{
    q->front = 0;
    q->rear = 0;
}