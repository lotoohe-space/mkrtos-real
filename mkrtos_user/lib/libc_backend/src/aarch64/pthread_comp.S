.text
.global __pthread_new_thread_entry__
.type   __pthread_new_thread_entry__,%function
__pthread_new_thread_entry__:
    ldr x0, [sp, #12]
    ldr x1, [sp, #4]
    br x1

