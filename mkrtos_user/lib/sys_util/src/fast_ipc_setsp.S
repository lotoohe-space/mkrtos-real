.syntax unified
.thumb
.global fast_ipc_setsp
/**
* @brief fast_ip_setsp
*/
.type fast_ipc_setsp, %function
fast_ipc_setsp:
	mov sp, r1
	bx lr