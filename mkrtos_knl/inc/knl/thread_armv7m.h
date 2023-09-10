#pragma once

void thread_exit(void);
void thread_user_pf_set(thread_t *cur_th, void *pc, void *user_sp, void *ram, umword_t stack);