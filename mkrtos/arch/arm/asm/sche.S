.syntax unified
.cpu cortex-m3
.thumb

.global PendSV_Handler
.section .text.Reset_Handler
.type PendSV_Handler, %function
PendSV_Handler:
	CPSID   I

	//psp��msp��Ϊ�����л��Ĳ���
	MRS R0,PSP
	MRS R1,MSP
	cmp r0,#0

	//������ǵ�һ�ε����ˣ��򱣴�r4-r11��ջ��
	TST LR,#4
	ITE EQ
	MRSEQ r3, MSP
	MRSNE r3, PSP
	ITE EQ
	MOVEQ r2, #0
	MOVNE r2, #1


	//���psp���Ϊ0��˵���ǵ�һ�ν��е��ȣ������̽���һ�ε���
	CMP r0,#0
	CBZ r0, thread_change

	SUB R3, R3, #0x20 //R0 -= 0x20
	STM R3, {R4-R11}
	//����2 ����ʹ��ջ��Ϊpsp
	//mov r2,#1
	//#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
	//;֧��FPU
	//		SUB     R3, R3, #0x40
	//		VSTM    R3, {D8-D15}
	//#endif
	//�����ĵ�ֵ�Ż�Ҫ���Ĳ�����ȥ
	TST LR,#4
	ITE EQ
	MOVEQ r1,r3
	MOVNE r0,r3
	ITE EQ
	MSREQ MSP, r3
	MSRNE PSP, r3

thread_change://�������
	//r0:psp r1:msp  r2:(0Ϊmsp 1 psp)
	ldr.w r3,=sys_task_sche
	blx r3
	//ȡ����һ������Ĳ��� r1 psp r2 msp
	ldrd r1,r2,[r0]
	//�û�ʹ�õ�ջ����r3 0Ϊmsp 1Ϊpsp 2��־����Ϊ�ں�����Ĭ��ʹ��msp
	ldr r3,[R0,#8]

	//���ʹ�õļĴ���
	tst r3,#1
	ITE EQ
	MOVEQ R0,R2
	MOVNE R0,R1//����������ʹ��psp
	//#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
	//;֧��FPU
	//		VLDM 		R0, {D8-D15} //�ָ��µ�r4-r11��ֵ
	//		ADDS 		R0, R0, #0x40 //R0 += 0x40
	//#endif
	LDM r0, {R4-R11} 		//�ָ��µ�r4-r11��ֵ
	ADDS r0, r0, #0x20 	//R0 += 0x20

	//#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
	//BIC.W   LR, LR, #0x10
	//#endif
	//����ջ��ֵ��ȥ
	MSR MSP,R2
	MSR PSP,R1

	//�������2����˵�����ں��߳�
	//����ת��re
	cmp r3,#2
	beq ree

	//���svc�Ƿ�Ƕ��
	tst r3,#0x00010000
	bne reeSvc//Ƕ������ִ����������

	//û��Ƕ�ף�������ʹ�õ�ջ
	cmp r3,#1
	beq userMode//pspģʽ����ת
	b ree
userMode:
//pspģʽ�򱣴�
	MSR PSP, r0
	//�л����û��߳�ģʽ
	MOV R1,#1
	MSR CONTROL, R1
	//lr �ĵ�2λΪ1ʱ�Զ��л���PSP
	MOV LR,#0xfffffffd
	b ree1

////�ں��߳�
reeSvc:
	//�����2�����ں��̱߳��ж�
	tst r3,#0x00000002
	bne setKenTher //��ת����Ϊ��Ȩģʽ
	//��������Ϊ����Ȩģʽ
	MOV R1,#3
	MSR CONTROL, R1
	b setKenTherNext
setKenTher:
	MOV R1,#0
 	MSR CONTROL, R1
setKenTherNext:
//	ldr.w r1,=SetSvcStatus
//	blx r1
	MSR MSP,R0
	MOV LR,#0xfffffff1
	b ree1

ree: //������msp��ֵ
	MSR MSP,R0
	MOV LR,#0xfffffff9
	//�ں��̣߳�����Ϊ��Ȩģʽ
	MOV R0,#0
	MSR CONTROL, R0
ree1:
	//���ж�
	cpsie I
	BX 	LR