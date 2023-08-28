#include "types.h"

int ffs(int x)
{
    int ret;

    __asm__ volatile("clz\t%0, %1"
                     : "=r"(ret)
                     : "r"(x)
                     : "cc");
    ret = (WORD_BITS - 1) - ret;
    return ret;
}
