.syntax unified
.text

/*
int mk_syscall(int nr, ...)
*/
.global mk_syscall
.type mk_syscall, %function
mk_syscall:
    push {r4-r7}
    mov r7, r0
    mov r0, r1
    mov r1, r2
    mov r2, r3
    add r6, sp,#16
    ldmia r6,{r3-r5}
    svc 128
    pop {r4-r7}
    bx lr
