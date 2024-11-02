.syntax unified
@ .cpu cortex-m3
.thumb

.global syscall_handler_get
/**
* @brief syscall结束时到这儿
*/
.type syscall_exit, %function
syscall_exit:
	svc 1
	b .

/**
* void syscall_ret_user(int ret_val);
* @brief 回到用户模式
*/
.type syscall_ret_user, %function
syscall_ret_user:
	//存放返回值

	//还原msp的值
	mrs		r0, msp
#ifdef MKRTOS_USE_FPU
	// 如果支持浮点，则里则要把惰性浮点得位置留查出来
	TST lr, #0x10
	IT eq
	addeq r0,r0,#0x48
#endif
	add		r0, #0x20
	ldmfd	r0!,{r4-r11}
	msr		msp,r0
	pop 	{r0,r1,r2,lr}

    msr     CONTROL, r1
	CPSIE I
    bx      lr

/**
* void SVC_Handler(void);
* @brief
*/
.global SVC_Handler
.type SVC_Handler, %function
SVC_Handler:
	CPSID   I
	TST.W LR, #4
	ITE EQ
 	MRSEQ R1, MSP
	MRSNE R1, PSP
	//获得svc的调用号
	LDR     R2, [R1, #24]
	LDRB    R2, [R2, #-2]

	cmp r2,#128
	beq SYS_CALL
	cmp r2,#1
	beq TO_USER
	cmp r2,#2
//  beq enter_fchannel
//	cmp r2,#3
//	beq exit_fchannel
	b TO_END
TO_USER:
	B syscall_ret_user

SYS_CALL:
	//获取堆栈地址
	mov r0,r1
	//放入之前的栈地址
	mrs r1,CONTROL

	push {r0,r1,r2,lr}
    //svc 128
	mov r1, r7
	//获得需要调用的syscall函数的地址
	push {r0-r2,lr}
	blx	syscall_handler_get
	mov r3,r0
	pop {r0-r2,lr}

	//r4-r11 压入到内核栈中
	stmfd sp!,{r4-r11}
	//压入到用户栈中
	stmfd r0,{r4-r11}
#ifdef MKRTOS_USE_FPU
	// 如果支持浮点，则里则要把惰性浮点得位置留查出来
	TST lr, #0x10
	IT eq
	subeq sp,sp,#0x48
#endif
	//取出PSR PC LR R12 R3 R2 R1 R0
	ldmfd r0,{r4-r11}
	//压入到内核栈中
	stmfd sp!,{r4-r11}

	//取出lr与pc放到栈中
	mov		r0,sp
	ldr     r2, =syscall_exit
	//lr
	str     r2, [r0, #20]
	//pc
	str     r3, [r0, #24]
	ldr		r3, =0x01000000
	str		r3, [r0, #28]

	//重新设置control
    mrs     r0, CONTROL
    bic     r0, r0, #0x03
    msr     CONTROL, r0

    dsb
    isb
    orr     lr, lr, #0x08
    bic		lr, lr, #0x04
	CPSIE I
    bx      lr

TO_END:
CPSIE I
	bx	lr
