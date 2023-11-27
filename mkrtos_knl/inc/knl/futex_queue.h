#pragma once

#include "types.h"

#define FUTEX_QUEUE_LEN 16 //!< 一个地址最多有16个等待者

typedef struct futex_queue
{
    umword_t m[FUTEX_QUEUE_LEN];
    int front;
    int rear;
} futex_queue_t;
typedef int (*futex_iter_func)(umword_t e, void *data);

void fq_init(futex_queue_t *q);
int fq_empty(futex_queue_t *q);
int fq_enqueue(futex_queue_t *q, umword_t e);
int fq_dequeue(futex_queue_t *q, umword_t *e);
int fq_queue_len(futex_queue_t *q);
bool_t fq_is_full(futex_queue_t *q);
int fq_queue_iter(futex_queue_t *q, futex_iter_func iter_func, void *data);
