.syntax unified
.cpu cortex-m3
.thumb

.global atomic_inc
.section .text.atomic_inc
.type atomic_inc, %function
atomic_inc:
    	MOV R3,#0
        TryInc:
        	LDREX r2, [R0]
        	ADD r2, #1
        	CMP R2,#0
        	BEQ incN
        	B incN1
        incN:
        	MOV R3,#1
        incN1:
        	STREX R1, R2, [R0]
        	CMP R1, #1	//��� STREX �Ƿ񱻲���
        	BEQ TryInc	//���³���
        	MOV R0,R3
        	bx lr

.global atomic_dec
.section .text.atomic_dec
.type atomic_dec, %function
atomic_dec:
        MOV R3,#0
        TryDec:
            LDREX r2, [R0]
            ADD r2, #-1
            CMP R2,#0
            BEQ decN
            B decN1
        decN:
            MOV R3,#1
        decN1:
            STREX R1, R2, [R0]
            CMP R1, #1	//��� STREX �Ƿ񱻲���

            BEQ TryDec	//���³���

            bx lr
.global atomic_set
.section .text.atomic_set
.type atomic_set, %function
atomic_set:
TrySet:
	LDREX r2, [R0]
	MOV r2, R1
	STREX R3, R2, [R0]
	CMP R3, #1	//��� STREX �Ƿ񱻲���
	BEQ TrySet	//���³���

	bx lr

.global atomic_ret_set
.section .text.atomic_ret_set
.type atomic_ret_set, %function
atomic_ret_set:
	push {r4}
TryRetSet:
	LDREX r2, [R0]
	mov r4,r2
	MOV r2, R1
	STREX R3, R2, [R0]
	CMP R3, #1	//��� STREX �Ƿ񱻲���
	BEQ TryRetSet	//���³���
	mov r0,r4
	pop {r4}
	bx lr

.global atomic_read
.section .text.atomic_read
.type atomic_read, %function
atomic_read:
	LDR R0, [R0]
	BX LR

.global atomic_add
.section .text.atomic_add
.type atomic_add, %function
atomic_add:
	MOV R3,R1
TryAdd:
	LDREX r2, [R0]
	ADD r2, R3
	STREX R1, R2, [R0]
	CMP R1, #1	//��� STREX �Ƿ񱻲���
	BEQ TryAdd	//���³���
	BX LR

.global atomic_sub
.section .text.atomic_sub
.type atomic_sub, %function
atomic_sub:
	MOV R3,R1
TrySub:
	LDREX r2, [R0]
	SUB r2, R3
	STREX R1, R2, [R0]
	CMP R1, #1	//��� STREX �Ƿ񱻲���
	BEQ TrySub	//���³���
	BX LR

.global atomic_test
.section .text.atomic_test
.type atomic_test, %function
atomic_test:
	push {r4}
TryTest:
	LDREX r2, [R0]
	CMP R2,r1 //�Ƚ��Ƿ����
	BEQ FZTest //��������ת����ֵ
	MOV R4,#0
	B FZETest
FZTest:
	MOV R4,#1
FZETest:
	STREX R3, R2, [R0]
	CMP R3, #1	//��� STREX �Ƿ񱻲���

	BEQ TryTest	//���³���
	MOV R0,R4
	pop {r4}
	bx lr

.global atomic_test_set
.section .text.atomic_test_set
.type atomic_test_set, %function
atomic_test_set:
	push {r4}
TryTestSet:
	LDREX r2, [R0]
	CMP R2,#0 //�Ƚ��Ƿ����0
	BEQ FZTestSet //����0����ת����ֵ
	MOV R4,#0
	B FZETestSet
FZTestSet:
	MOV R4,#1
	MOV r2, R1 //����������и�ֵ
FZETestSet:
	STREX R3, R2, [R0]
	CMP R3, #1	//��� STREX �Ƿ񱻲���

	BEQ TryTestSet	//���³���
	MOV R0,R4
	pop {r4}
	bx lr

.global atomic_set_test
.section .text.atomic_set_test
.type atomic_set_test, %function
atomic_set_test:
	push {r4}
TrySetTest:
	LDREX r2, [R0]
	CMP R2,#0 //�Ƚ��Ƿ����0
	BEQ FZSetTest //����0����ת����ֵ
	MOV R4,#0
	B FZESetTest
FZSetTest:
	MOV R4,#1

FZESetTest:
    MOV r2, R1 //����������и�ֵ
	STREX R3, R2, [R0]
	CMP R3, #1	//��� STREX �Ƿ񱻲���

	BEQ TrySetTest	//���³���
	MOV R0,R4
	pop {r4}
	bx lr

.global atomic_inc_test
.section .text.atomic_inc_test
.type atomic_inc_test, %function
atomic_inc_test:
        push {r4}
    TryIncTest:
        LDREX r2, [R0]
        CMP R2,#0 //�Ƚ��Ƿ����0
        BEQ FZIncTest //����0����ת����ֵ
        MOV R4,#0
        B FZEIncTest
    FZIncTest:
        MOV R4,#1
    FZEIncTest:
        ADD R2,R2,#1
        STREX R3, R2, [R0]
        CMP R3, #1	//��� STREX �Ƿ񱻲���

        BEQ TryIncTest	//���³���
        MOV R0,R4
        pop {r4}
        bx lr


.global atomic_test_inc
.section .text.atomic_test_inc
.type atomic_test_inc, %function
atomic_test_inc:
	push {r4}
TryTestInc:
	LDREX r2, [R0]
	CMP R2,#0 //�Ƚ��Ƿ����0
	BEQ FZTestInc //����0����ת����ֵ
	MOV R4,#0
	B FZETestInc
FZTestInc:
	MOV R4,#1
	ADD r2,r2,#1 //����������и�ֵ
FZETestInc:
	STREX R3, R2, [R0]
	CMP R3, #1	//��� STREX �Ƿ񱻲���

	BEQ TryTestInc	//���³���
	MOV R0,R4
	pop {r4}
	bx lr

.global atomic_test_dec_nq
.section .text.atomic_test_dec_nq
.type atomic_test_dec_nq, %function
atomic_test_dec_nq:
	push {r4}
TryTestDecNq:
	LDREX r2, [R0]
	CMP R2,#0 //�Ƚ��Ƿ����0
	BEQ FZTestDecNq//����0����ת
	MOV R4,#0
	ADD r2, #-1 //�����������һ
	B FZETestDecNq
FZTestDecNq:
	MOV R4,#1
FZETestDecNq:
	STREX R3, R2, [R0]
	CMP R3, #1	//��� STREX �Ƿ񱻲���

	BEQ TryTestDecNq	//���³���
	MOV R0,R4
	pop {r4}
	bx lr

//b<=a a+=1
.global atomic_cmp_hi_inc
.section .text.atomic_cmp_hi_inc
.type atomic_cmp_hi_inc, %function
atomic_cmp_hi_add:
    push {r4}
atomic_cmp_hi_add_:
    LDREX r2, [r0]
    cmp r1,r2 //b,a
    bls atomic_cmp_hi_add_nx_
    MOV r4,#0
atomic_cmp_hi_add_nx_:
    ADD r2,r2,#1
    MOV r4,#1
    STREX r3,r2,[r0]
    CMP R3, #1
    BEQ atomic_cmp_hi_add_
    MOV R0,R4
    pop {r4}
    bx lr

//a<=b a+=1
.global atomic_cmp_hi_inc1
.section .text.atomic_cmp_hi_inc1
.type atomic_cmp_hi_inc1, %function
atomic_cmp_hi_add1:
    push {r4}
atomic_cmp_hi_add1_:
    ldrex r2,[r0]
    cmp r2,r1
    bls atomic_cmp_hi_add1_nx_
    MOV r4,#0
atomic_cmp_hi_add1_nx_:
    ADD r2,r2,#1
    MOV r4,#1
    STREX r3,r2,[r0]
    CMP R3, #1
    BEQ atomic_cmp_hi_add1_
    MOV R0,R4
    pop {r4}
    bx lr

