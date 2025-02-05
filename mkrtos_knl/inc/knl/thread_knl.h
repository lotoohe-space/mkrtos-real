#pragma once

#include <task.h>
void thread_calc_cpu_usage(void);
uint16_t cpu_get_current_usage(void);
bool_t task_knl_kill(thread_t *kill_thread, bool_t is_knl);
void knl_init_1(void);
