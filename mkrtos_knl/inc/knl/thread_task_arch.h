#pragma once
void thread_knl_pf_set(thread_t *cur_th, void *pc);
void thread_user_pf_set(thread_t *cur_th, void *pc, void *user_sp, void *ram);
void thread_user_pf_restore(thread_t *cur_th, void *user_sp);
void thread_set_user_pf_noset_knl_sp(thread_t *cur_th, void *pc, void *user_sp, void *ram);
void task_knl_init(task_t *knl_tk);
