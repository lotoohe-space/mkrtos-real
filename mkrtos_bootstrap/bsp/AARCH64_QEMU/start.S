
/* 8字节对齐，否则会出错*/
.align 3
.globl sp_addr
.section ".data.boot"
sp_addr:
	.quad stack + 4096 - 8 //pt regs is 320 bytes.

.align 3
.globl knl_entry
.section ".data.boot"
knl_entry:
	.quad 0

.section ".text.boot"
.globl _start
_start:  
	msr daifset, #0xf	// 关闭中断
    //进入多核启动流程
	mrs	x0, mpidr_el1	// 读取该寄存器获取处理器id
	and	x0, x0,#0xFF	// Check processor id
	cbz	x0, master		// Hang for all non-primary CPU
		//spin table
	ldr x1, = knl_entry
spin_jump:
	ldr x3, [x1, #0]
	cbz x3, spin_jump
	br x3
	b .
	
master:
    ldr x8, = sp_addr
	ldr x9, [x8]
 	mov sp, x9
	bl main

    b .