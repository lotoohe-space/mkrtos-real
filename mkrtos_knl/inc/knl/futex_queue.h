#pragma once

#include "types.h"

#define FUTEX_QUEUE_LEN 16 //!< 一个地址最多有16个等待者

typedef struct futex_queue
{
    umword_t m[FUTEX_QUEUE_LEN];
    int front;
    int rear;
} futex_queue_t;

void fq_init(futex_queue_t *q);
int fq_empty(futex_queue_t *q);
int fq_enqueue(futex_queue_t *q, umword_t e);
int fq_dequeue(futex_queue_t *q, umword_t *e);
int fq_queue_len(futex_queue_t *q);
