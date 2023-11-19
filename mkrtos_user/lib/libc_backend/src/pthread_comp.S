.syntax unified
.text
.global __pthread_new_thread_entry__
.type   __pthread_new_thread_entry__,%function
__pthread_new_thread_entry__:
    ldr r0, [sp, #12]
    ldr r1, [sp, #4]
    bx r1

