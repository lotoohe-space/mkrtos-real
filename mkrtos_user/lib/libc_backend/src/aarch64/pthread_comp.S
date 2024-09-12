.text
.global __pthread_new_thread_entry__
.type   __pthread_new_thread_entry__,%function
__pthread_new_thread_entry__:
    ldr x0, [sp, #16]
    ldr x1, [sp, #0]
    br x1

