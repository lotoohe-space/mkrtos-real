#pragma once
void thread_knl_pf_set(thread_t *cur_th, void *pc);
void thread_user_pf_set(thread_t *cur_th, void *pc, void *user_sp, void *ram, umword_t stack);
void task_knl_init(task_t *knl_tk);
