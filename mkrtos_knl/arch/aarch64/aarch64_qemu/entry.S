#include <asm/sysregs.h>
#include <asm_config.h>
#include <complier.h>

.section  .rodata
.align 3
.globl string1
string1:
	.string "Booting at EL"

/* 8字节对齐，否则会出错*/
.align 3
.globl sp_addr
.section ".data.boot"
sp_addr:
	.quad thread_knl_stack + CONFIG_THREAD_BLOCK_SIZE - 8//pt regs is 320 bytes.

.align 3
.globl cpio_images
.section ".data.boot"
cpio_images:
	.quad 0

.align 3
.global cpu_jump_addr
.section ".data.boot"
cpu_jump_addr:
	.zero (CONFIG_CPU * 8)

.align 3
.global per_cpu_stack_addr
.section ".data.boot"
per_cpu_stack_addr:
	.zero (CONFIG_CPU * 8)

.section ".text.boot"
.globl _start
_start:
	msr daifset, #0xf	// 关闭中断
	mov x9, x0

	mrs x5, CurrentEL
	cmp x5, #CurrentEL_EL3
	b.eq el3_entry //进入el3的处理函数
	b el2_entry

el3_entry:
	ldr x0, = SCTLR_EL2_VALUE_MMU_DISABLED
	msr sctlr_el2, x0

	ldr x0, =((1UL) | (1UL<<7) | (1UL<<8) | (1UL<<10))
	msr scr_el3, x0

	ldr x0, =SPSR_EL2
	msr spsr_el3, x0

	adr x0, el2_entry
	msr elr_el3, x0

	eret

el2_entry: //进入el2

	mrs x0, cpacr_el1
	orr x0, x0, 0x3 << 20 // SIMD不关心
	msr cpacr_el1, x0

	/* 设置异常向量表基地址到vbar寄存器 */
	ldr     x5, =vectors
	msr     vbar_el2, x5
	isb

	//进入多核启动流程
	mrs	x0, mpidr_el1	// 读取该寄存器获取处理器id
	and	x0, x0,#0xFF	// Check processor id
	cbz	x0, master		// Hang for all non-primary CPU

	//spin table
	ldr x1, = cpu_jump_addr
	lsl x2, x0, MWORD_SHIFT
spin_jump:
	ldr x3, [x1, x2]
	cbz x3, spin_jump
	ldr x4, = per_cpu_stack_addr
	ldr x4, [x4, x2] //获取sp启动的值
	mov sp, x4 //cpu启动的sp地址
	br x3
	b .
master:
	ldr x0, = cpio_images
	str x9, [x0, #0]
	/* set sp to top of init_task_union */
	ldr x8, =sp_addr
	ldr x9, [x8]
 	mov sp, x9
	bl boot_mapping
proc_hang:
	b proc_hang // should never come here

.globl jump_kenel_main
jump_kenel_main:
	ldr x1, = start_kernel
	br x1
	b .

.section ".text"

BEGIN_FUNC(per_cpu_boot_func)
	//进入多核启动流程
	mrs	x0, mpidr_el1	// 读取该寄存器获取处理器id
	and	x0, x0,#0xFF	// Check processor id
	cbz	x0, percpu_spin_loop		// Hang for all non-primary CPU

	//spin table
	ldr x1, = cpu_jump_addr
	lsl x2, x0, MWORD_SHIFT
percpu_spin_jump:
	ldr x3, [x1, x2]
	cbz x3, percpu_spin_jump
	ldr x4, = per_cpu_stack_addr
	ldr x4, [x4, x2] //获取sp启动的值
	mov sp, x4 //cpu启动的sp地址
	br x3
percpu_spin_loop:
	b .
END_FUNC(per_cpu_boot_func)
