.syntax unified
@ .cpu cortex-m3
.thumb

.global mpu_switch_to
.global scheduler

.global PendSV_Handler
.type PendSV_Handler, %function
PendSV_Handler:
	CPSID   I

	// psp和msp作为任务切换的参数
	MRS R0,PSP
	MRS R1,MSP

	// 如果不是第一次调度了，则保存r4-r11到栈内
	TST LR,#4
	ITE EQ
	MRSEQ r3, MSP
	MRSNE r3, PSP
	mov r2, lr

	push {r4}
	ldr r4, =scheduler
	ldr r4, [r4]
	//! psp为零时，立刻调度一次
	CBZ r4, thread_sche
	pop {r4}
	SUB R3, R3, #0x20 //R3 -= 32
	STM R3, {R4-R11}

	//将更改的值放回要传的参数中去
	TST LR,#4
	ITTEE EQ
	MSREQ MSP, r3
	MOVEQ r1,r3
	MSRNE PSP, r3
	MOVNE r0,r3
	b thread_sche_next
thread_sche:
	pop {r4}
thread_sche_next:
	// r0:psp r1:msp r2:(0为msp 1 psp)
	LDR.w r3,=schde_to
	BLX r3
	//取得下一个任务的参数 r1 psp r2 msp
	lDRD r1,r2,[r0]
	//用户使用的栈类型r3 0为msp 1为ps
	LDR r3,[R0,#8]

	TST r3,#4				//检测使用的寄存器
	ITE EQ
	MOVEQ R0,R2
	MOVNE R0,R1				//不等于零则使用psp
	LDM r0, {R4-R11} 		//恢复新的r4-r11的值
	ADDS r0, r0, #0x20 		//R0 += 0x20

	//保存栈的值回去
	MSR MSP,R2
	MSR PSP,R1

	//等于一跳转到用户模式
	@ CMP r3,#1
	TST r3,#4				//检测使用的寄存器
	IT EQ
	BEQ super_mode//psp模式则跳转
	B user_mode
user_mode:
//psp模式则保存
	MSR PSP, r0
	//切换到用户线程模式
	MOV R1,#1
	MSR CONTROL, R1
	//lr 的第2位为1时自动切换到PSP
	MOV LR,r3
	B to_ret

super_mode: //更新下msp的值
	MSR MSP,R0
	MOV LR,r3
	//内核线程，设置为特权模式
	MOV R0,#0
	MSR CONTROL, R0
to_ret:
	push {r0-r3, lr}
	bl mpu_switch_to
	pop {r0-r3, lr}
	CPSIE I
	DSB
	ISB
	BX 	LR
