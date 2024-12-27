.syntax unified
@ .cpu cortex-m3
.thumb
.global setsp
.global last_process
/**
* @brief syscall结束时到这儿
*/
.type setsp, %function
setsp:
	mov sp, r1
	bl last_process