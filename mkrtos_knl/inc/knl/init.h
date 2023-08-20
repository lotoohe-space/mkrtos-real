#pragma once

void sys_call_init(void);

#define INIT_LOW_HARD __attribute__((constructor(101)))
#define INIT_HIGH_HAD __attribute__((constructor(102)))
#define INIT_MEM __attribute__((constructor(103)))
#define INIT_KOBJ_MEM __attribute__((constructor(104)))
#define INIT_KOBJ __attribute__((constructor(105)))
#define INIT_STAGE1 __attribute__((constructor(106)))
#define INIT_STAGE2 __attribute__((constructor(107)))
