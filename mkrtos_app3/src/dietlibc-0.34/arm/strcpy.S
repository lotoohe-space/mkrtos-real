#include "dietfeatures.h"
#include "arm-features.h"

FUNC_START	strcpy
#ifndef WANT_SMALL_STRING_ROUTINES
#ifdef WANT_VALGRIND_SUPPORT
	ldr	ip, =__valgrind
	bne	.Lvalgrind
#endif

	mov	ip, r0
	ands	r2, r1, #3
	push {r4}
	mov r4,#-1
	mul r2,r2,r4
	add r1,r4
	ldr	r3, [r1, r4]
	push {r4}
	it ne
	movne	r2, r2, lsl #3
#ifdef __ARMEB__
it ne
	movne	r3, r3, lsl r2
#else
it ne
	movne	r3, r3, lsr r2
#endif
	it ne
	addne	pc, pc, r2, lsl #1
	mov	r0, r0			@ nop
.Lloop:
#ifdef __ARMEB__
	mov	r3, r3, ror #24
	tst	r3, #255
	strb	r3, [ip], #1
	moveq	pc, lr
	mov	r3, r3, ror #24
	tst	r3, #255
	strb	r3, [ip], #1
	moveq	pc, lr
	mov	r3, r3, ror #24
	tst	r3, #255
	strb	r3, [ip], #1
	moveq	pc, lr
	mov	r3, r3, ror #24
	tst	r3, #255
	strb	r3, [ip], #1
	ldrne	r3, [r1, #4]!
#else
	tst	r3, #255
	strb	r3, [ip], #1
	it eq
	moveq	pc, lr
	mov	r3, r3, lsr #8
	tst	r3, #255
	strb	r3, [ip], #1
	it eq
	moveq	pc, lr
	mov	r3, r3, lsr #8
	tst	r3, #255
	strb	r3, [ip], #1
	it eq
	moveq	pc, lr
	mov	r3, r3, lsr #8
	tst	r3, #255
	strb	r3, [ip], #1
	it ne
	ldrne	r3, [r1, #4]!
#endif
	bne	.Lloop
#ifdef WANT_VALGRIND_SUPPORT
	RET

.Lvalgrind:
#endif
#endif

#if defined(WANT_SMALL_STRING_ROUTINES) || defined(WANT_VALGRIND_SUPPORT)
	ldrb	r2, [r1], #1
	mov	r3, r0
.Lslowloop:
	strb	r2, [r3], #1
	tst	r2, #255
	it neb
	ldrneb	r2, [r1], #1
	bne	.Lslowloop
#endif
	RET
FUNC_END	strcpy
