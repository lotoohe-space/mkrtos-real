#pragma

#include "thread.h"
typedef struct thread_wait_entry
{
    slist_head_t node;
    thread_t *th;
    umword_t times;
} thread_wait_entry_t;
void thread_check_timeout(void);
thread_wait_entry_t *thread_sleep_del(thread_t *th);
void thread_sleep_del_and_wakeup(thread_t *th);
umword_t thread_sleep(umword_t tick);
