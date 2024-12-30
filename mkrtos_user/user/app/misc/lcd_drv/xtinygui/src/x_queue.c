/**
 * @file x_queue.c
 * @author ATShining (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "x_queue.h"
#include "x_malloc.h"
#include <pthread.h>

/*初始化队列*/
uint8_t InitQueue(xqueue_t *q, uint16_t size)
{
	QueueDateType *data;
	if (q == NULL)
	{
		return FALSE;
	}
	data = XMalloc(sizeof(QueueDateType) * size);
	if (data == NULL)
	{
		return FALSE;
	}
	q->maxVal = size;
	q->data = data;
	q->front = q->rear = 0;
	q->valid = TRUE;
	pthread_mutex_init(&q->lock, NULL);
	return TRUE;
}
uint16_t QueueLength(xqueue_t *q)
{
	if (q == NULL)
	{
		return 0;
	}
	return (q->rear - q->front + q->maxVal) % q->maxVal;
}
void DestroyQueue(xqueue_t *q)
{
	if (q == NULL)
	{
		return;
	}
	XFree(q->data);
}
uint8_t QueueEmpty(xqueue_t *q)
{
	if (q == NULL)
	{
		return 1;
	}
	return (q->front == q->rear);
}
uint8_t enQueue(xqueue_t *q, QueueDateType e)
{
	pthread_mutex_lock(&q->lock);
	if ((q->rear + 1) % q->maxVal == q->front)
	{
		pthread_mutex_unlock(&q->lock);
		return FALSE;
	}
	q->rear = (q->rear + 1) % q->maxVal;
	q->data[q->rear] = e;
	pthread_mutex_unlock(&q->lock);
	return TRUE;
}
uint8_t deQueue(xqueue_t *q, QueueDateType *e)
{
	pthread_mutex_lock(&q->lock);
	if (q->front == q->rear)
	{
		/*空了，则返回错误*/
		pthread_mutex_unlock(&q->lock);
		return FALSE;
	}
	q->front = (q->front + 1) % q->maxVal;
	*e = q->data[q->front];
	pthread_mutex_unlock(&q->lock);
	return TRUE;
}
/*获取队尾的元素*/
uint8_t getTailQueue(xqueue_t *q, QueueDateType *e)
{
	pthread_mutex_lock(&q->lock);
	if (q->front == q->rear)
	{ /*空了，则返回错误*/
		pthread_mutex_unlock(&q->lock);
		return FALSE;
	}
	*e = q->data[(q->front + 1) % q->maxVal];
	pthread_mutex_unlock(&q->lock);
	return TRUE;
}
