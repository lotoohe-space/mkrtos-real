#include "arm-features.h"
.syntax unified
.cpu cortex-m3
.thumb

FUNC_START	__longjmp
	mov	ip, r0
	movs	r0, r1
	//ATShining
	it		eq
	moveq	r0, #1
#ifndef __SOFTFP__
# if __ARM_ARCH__ >= 6
	vldm	ip!, {d0-d15}
#   ifdef __ARM_NEON__
	vldm	ip!, {d16-d31}
#   endif
# else
	lfm	f4, 4, [ip]!
# endif
#endif

#ifdef __IWMMXT__
#  warning "sigjmp will not restore iwmmxt coprocessor registers"
#endif

	//ATShining ldmia   ip!, {r4-r11, sp, pc}
	ldmia   ip!, {r4-r11}
    ldmia   ip!, {r2}
    mov sp,r2
    ldmia   ip!, {pc}
FUNC_END	__longjmp
