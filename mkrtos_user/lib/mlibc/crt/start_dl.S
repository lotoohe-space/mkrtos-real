    .syntax unified
    .section .first
    
    @ .globl reloc

    .align 2
    .globl    _dlstart
    .type    _dlstart, %function
_dlstart:
    /*save r0 */
    mov     r12, r0
    b       __start
    .SPACE   32 - (. -  _dlstart)
_head_start:
    .ascii  "MKRTOS. "
    .word   __ram_size__
heap_offset:     .word     __heap_start__ - _dlstart
stack_offset:    .word     __stack_start__ - _dlstart
heap_size:       .word     __heap_size__
stack_size:      .word     __stack_size__
data_offset:     .word     __data_start__ - _dlstart

    .word   __bss_start__ - _dlstart
got_start:    	.word    	__got_start__
got_end:       	.word     	__got_end__
rel_start:      .word     	__rel_start__
rel_end:     .word    __rel_end__
text_start: .word __text_start__
    .SPACE   128 - (. -  _dlstart)

    .align
__start:

    movs    r1, #0
    adr     r2, _dlstart
    bic     r2, r2, #1
    ldr     r0, = __data_start__
    add     r0, r0, r2
    ldr     r3, = __data_end__
    add     r3, r3, r2
    b       LoopCopyDataInit
CopyDataInit:
    ldr     r2, [r0, r1]
    str     r2, [r9, r1]
    adds    r1, r1, #4
LoopCopyDataInit:
    adds    r2, r0, r1
    cmp     r2, r3
    bcc     CopyDataInit

    @ mov     r0, r9
    @ adr     r1, _dlstart
    @ bic     r1, r1, #1
    @ bl      _reloc

	@ pop     {r0, r12}
    @ mov     r0, sp
    @ adr		r1, _dlstart

	mov fp, #0
	mov lr, #0
	ldr a2, 1f
	add a2, pc, a2
	mov a1, sp
2:	and ip, a1, #-16 
	mov sp, ip
	bl _dlstart_c
.weak _DYNAMIC
.hidden _DYNAMIC
.align 2 
1:	.word _DYNAMIC-2b
    b		.
	.end
