#pragma once
#include <types.h>
/*
 * PSR bits
 */
#define PSR_MODE_EL0t	0x00000000
#define PSR_MODE_EL1t	0x00000004
#define PSR_MODE_EL1h	0x00000005
#define PSR_MODE_EL2t	0x00000008
#define PSR_MODE_EL2h	0x00000009
#define PSR_MODE_EL3t	0x0000000c
#define PSR_MODE_EL3h	0x0000000d
#define PSR_MODE_MASK	0x0000000f

typedef struct entry_frame {
	struct {
		mword_t regs[31];
		mword_t sp;
		mword_t pc;
		mword_t pstate;
	};

	mword_t orig_x0;
	uint32_t syscallno;
	uint32_t unused2;

	mword_t orig_addr_limit;
	mword_t unused;
	mword_t stackframe[2];
} entry_frame_t;
