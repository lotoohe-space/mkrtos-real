.syntax unified
.thumb
.global fast_ipc_setsp
/**
* @brief syscall结束时到这儿
*/
.type fast_ipc_setsp, %function
fast_ipc_setsp:
	mov sp, r1
	bx lr