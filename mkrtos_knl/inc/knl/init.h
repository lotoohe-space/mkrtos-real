#pragma once
#include "util.h"
typedef void (*init_func)(void);

void sys_call_init(void);

#define INIT_LOW_HARD(f) static __USED __attribute__((__section__(".init_array." \
                                                                "102"))) init_func init_##f = f
#define INIT_HIGH_HAD(f) static __USED __attribute__((__section__(".init_array." \
                                                                "103"))) init_func init_##f = f
#define INIT_ONBOOT(f) static __USED __attribute__((__section__(".init_array." \
                                                                 "104"))) init_func init_##f = f
#define INIT_MEM(f) static __USED __attribute__((__section__(".init_array." \
                                                           "105"))) init_func init_##f = f
#define INIT_KOBJ_MEM(f) static __USED __attribute__((__section__(".init_array." \
                                                                "106"))) init_func init_##f = f
#define INIT_KOBJ(f) static __USED __attribute__((__section__(".init_array." \
                                                            "107"))) init_func init_##f = f
#define INIT_STAGE1(f) static __USED __attribute__((__section__(".init_array." \
                                                              "108"))) init_func init_##f = f
#define INIT_STAGE2(f) static __USED __attribute__((__section__(".init_array." \
                                                              "109"))) init_func init_##f = f
