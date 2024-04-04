.text

/*
int mk_syscall(int nr, ...)
*/
.global mk_syscall
.type mk_syscall, %function
mk_syscall:
    mov x7, x0
    mov x0, x1
    mov x1, x2
    mov x2, x3
    mov x3, x4
    mov x4, x5
    mov x5, x6
    hvc 0
    ret
