#include "types.h"
#include "init.h"

extern char *__init_array_start[];
extern char *__init_array_end[];

void sys_call_init(void)
{
    uintptr_t a = (uintptr_t)&__init_array_start;

    for (; a < (uintptr_t)&__init_array_end; a += sizeof(void (*)()))
        (*(void (**)(void))a)();
}
