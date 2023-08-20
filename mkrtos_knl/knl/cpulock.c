#include "types.h"
#include "arch.h"

umword_t cpulock_lock(void)
{
    umword_t res;

    res = intr_status();
    sti();
    return res;
}

umword_t cpulock_get_status(void)
{
    umword_t res;

    res = intr_status();
    return res;
}

void cpulock_set(umword_t s)
{
    if (s)
    {
        sti();
    }
    else
    {
        cli();
    }
}
