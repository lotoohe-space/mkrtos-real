.syntax unified
.cpu cortex-m3
.thumb

.global init
.section .text.init
.type init, %function
init:
CPSID   I
	//首先设置pendsv为最低优先级
	//设置pendsv的中断优先级
	ldr r0,=0xE000ED22
	//最低优先级,pendsv和systick优先级
	ldr r1,=0xffff
	strh r1,[r0]

	//ldr r0,=0xE000ED1F
	//ldr r1,=0xff
	//strb r1,[r0]
	//设置psp为0,用于判断是否第一次任务调度
	MOVS R0, #0 //R0 = 0
	MSR PSP, R0 //PSP = R0

	LDR R0, =0xE000ED04
	LDR R1, =0x10000000
	//进入pendsv中断
	STR R1, [R0]
	//打开中断
    CPSIE I ;
	//用户线程模式,用户以非特权模式运行
	MOV R0,#1
	MSR CONTROL, R0
	BX LR


