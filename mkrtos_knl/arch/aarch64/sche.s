#include <asm_config.h>
#include <asm_offset.h>


/* 进程切换： 保存prev进程的上下文，并且恢复next进程
的上下文
  cpu_switch_to(struct cpu_context *prev,
	   struct cpu_context *next);

需要保存的上下文： x19 ~ x29， sp， lr
保存到进程的task_struct->cpu_context
 */
.align
.global cpu_switch_to
cpu_switch_to:
	mov     x8, x0
	mov     x9, sp
	stp     x19, x20, [x8], #16
	stp     x21, x22, [x8], #16
	stp     x23, x24, [x8], #16
	stp     x25, x26, [x8], #16
	stp     x27, x28, [x8], #16
	stp     x29, x9, [x8], #16
	str     lr, [x8]

	mov     x8, x1
	ldp     x19, x20, [x8], #16
	ldp     x21, x22, [x8], #16
	ldp     x23, x24, [x8], #16
	ldp     x25, x26, [x8], #16
	ldp     x27, x28, [x8], #16
	ldp     x29, x9, [x8], #16
	ldr     lr, [x8]
	mov     sp, x9
	ret
