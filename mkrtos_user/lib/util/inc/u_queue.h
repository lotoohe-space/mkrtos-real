#pragma once

#include "u_types.h"

typedef struct queue
{
    uint8_t *m;
    int front;
    int rear;
    int size;
} queue_t;

void q_init(queue_t *q, uint8_t *data, int size);
int q_empty(queue_t *q);
int q_enqueue(queue_t *q, uint8_t e);
int q_dequeue(queue_t *q, uint8_t *e);
int q_queue_len(queue_t *q);
void q_queue_clear(queue_t *q);
