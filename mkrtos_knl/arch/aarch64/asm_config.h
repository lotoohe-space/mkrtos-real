#ifndef __ASM_CONFIG_H__
#define __ASM_CONFIG_H__

#include "asm/mm.h"
#define SUPPORT_SMP CONFIG_SMP
#define SYS_CPU_NUM CONFIG_CPU //!< 系统cpu数量

#define MWORD_BYTES (sizeof(mword_t))
#define MWORD_BITS (sizeof(mword_t) << 3UL)
#define MWORD_SHIFT (3)

#define PT_REGS_SIZE 320
#endif
