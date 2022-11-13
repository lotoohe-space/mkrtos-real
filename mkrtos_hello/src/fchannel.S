.syntax unified
.cpu cortex-m3
.thumb

//int fcall(int call_no, int arg0,int arg1, int arg2);
.global fcall
.type fcall, %function
fcall:
    push {r0-r3,lr}
    svc 2
    pop {r0-r3,lr}
    bx lr

/**
* 退出快速ipc，用户态调用
*/
.global fcall_exit
.type fcall_exit, %function
fcall_exit:
	svc 0x3
	bx lr
