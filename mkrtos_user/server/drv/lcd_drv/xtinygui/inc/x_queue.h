/**
 * @file x_queue.h
 * @author ATShining (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include "x_types.h"
#include <pthread.h>
typedef void *QueueDateType;

typedef struct
{
	QueueDateType *data;
	int16_t front, rear;
	uint16_t maxVal;
	pthread_mutex_t lock;
	uint8_t valid;
} *p_xqueue_t, xqueue_t;

uint8_t InitQueue(xqueue_t *q, uint16_t size);
void DestroyQueue(xqueue_t *q);
uint8_t QueueEmpty(xqueue_t *q);
uint8_t enQueue(xqueue_t *q, QueueDateType e);
uint8_t deQueue(xqueue_t *q, QueueDateType *e);
uint8_t getTailQueue(xqueue_t *q, QueueDateType *e);
