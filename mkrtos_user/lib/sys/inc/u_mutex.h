#pragma once

#include <u_types.h>

typedef struct u_mutex
{
    obj_handler_t obj;
} u_mutex_t;

#define U_MUTEX_INIT {.obj = HANDLER_INVALID}

int u_mutex_init(u_mutex_t *lock, obj_handler_t sema_hd);
void u_mutex_lock(u_mutex_t *lock, umword_t timeout, umword_t *remain_times);
void u_mutex_unlock(u_mutex_t *lock);
