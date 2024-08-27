#pragma once
#include <u_types.h>

void u_thread_del(obj_handler_t th_hd);
int u_thread_create(obj_handler_t *th_hd, void *stack, void *msg_buf, void (*thread_func)(void));
int u_thread_run(obj_handler_t th_hd, int prio);
int u_thread_run_cpu(obj_handler_t th_hd, int prio, int cpu);
