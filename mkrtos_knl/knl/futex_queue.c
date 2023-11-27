#include "types.h"
#include "futex_queue.h"
#include <assert.h>

void fq_init(futex_queue_t *q)
{
    q->front = 0;
    q->rear = 0;
}
int fq_empty(futex_queue_t *q)
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
bool_t fq_is_full(futex_queue_t *q)
{
    return (q->rear + 1) % FUTEX_QUEUE_LEN == q->front;
}
int fq_enqueue(futex_queue_t *q, umword_t e)
{
    if (((q->rear + 1) % FUTEX_QUEUE_LEN) == q->front)
    {
        return -1;
    }
    q->m[q->rear] = e;
    q->rear = (q->rear + 1) % FUTEX_QUEUE_LEN;
    return 0;
}
int fq_dequeue(futex_queue_t *q, umword_t *e)
{
    if (q->front == q->rear)
    {
        return -1;
    }
    *e = q->m[q->front];
    q->front = (q->front + 1) % FUTEX_QUEUE_LEN;
    return 0;
}
int fq_queue_len(futex_queue_t *q)
{
    return (q->rear - q->front + FUTEX_QUEUE_LEN) % FUTEX_QUEUE_LEN;
}
int fq_queue_iter(futex_queue_t *q, futex_iter_func iter_func, void *data)
{
    assert(iter_func);
    int i;
    int j = q->front;
    int e = (q->rear - q->front + FUTEX_QUEUE_LEN) % FUTEX_QUEUE_LEN;
    for (i = 0; i < e; i++)
    {
        int ret = iter_func(q->m[j], data);
        if (ret < 0)
        {
            return ret;
        }
        j = (j + 1) % FUTEX_QUEUE_LEN;
    }
}
