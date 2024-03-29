#include "arm-features.h"
.syntax unified
.cpu cortex-m3
.thumb
@
@ mcount.S: ARM assembler implementation of mcount
@
@ mcount builds the following stack frame:
@
@                  |             |
@                   -------------
@  sp on entry ->  |             |
@                   -------------
@  new fp ------>  | previous pc |
@                   -------------
@                  | previous lr |
@                   -------------
@                  | previous sp |
@                   -------------
@                  | previous fp |
@                   -------------
@                  | previous r3 |
@                   -------------
@                  | previous r2 |
@                   -------------
@                  | previous r1 |
@                   -------------
@  new sp -------> | previous r0 |
@                   -------------
@
@

FUNC_START	mcount
    mov     ip,  sp
    //stmdb   sp!, { r0 - r3, fp, ip, lr, pc }    @ build stack frame
    stmdb   sp!, { r0 - r3, fp, ip, lr}
    mov		r0, pc
    push	{r0}
    sub     fp,  ip, #4                         @ setup new fp

    ldr     r0,  [fp, #-12]                     @ r0 = previous fp
    ldr     r0,  [r0, #-4 ]                     @ r0 = pc from caller of mcount caller
    mov     r1,  lr                             @ r1 = pc from mcount caller

    bl      __mcount                            @ call __mcount

    //ldmdb   fp,  { r0 - r3, fp, sp, pc }        @ restore context from stack frame and return.
    mov r4,fp
	ldmdb	fp!, {r0-r3,r4}
	mov r0,sp
	ldmdb	fp!,{r0,pc}
FUNC_END	mcount
