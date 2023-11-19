#pragma once
#include <u_types.h>
int u_thread_create(obj_handler_t *th_hd, void *stack, umword_t stack_size, void *msg_buf, void (*thread_func)(void), int prio);
