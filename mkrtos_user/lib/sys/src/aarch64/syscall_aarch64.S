.text

/*
int mk_syscall(int nr, ...)
*/
.global mk_syscall
.type mk_syscall, %function
mk_syscall:
    hvc 0
    ret
