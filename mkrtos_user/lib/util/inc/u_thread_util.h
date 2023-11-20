#pragma once
#include <u_types.h>

void u_thread_del(obj_handler_t th_hd);
int u_thread_create(obj_handler_t *th_hd, void *stack, umword_t stack_size, void *msg_buf, void (*thread_func)(void), int prio);
