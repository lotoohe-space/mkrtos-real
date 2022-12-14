#include <arch/atomic.h>
#include <mkrtos/sched.h>
#include "config.h"
#include "knl_service.h"

static int test_sched(void)
{
    kprint("=======start sched mem\n");
    sche_lock();
    MKRTOS_ASSERT(sche_lock_cn_get() == 1);
    sche_unlock();
    MKRTOS_ASSERT(sche_lock_cn_get() == 0);

    for (int i = 0; i < 10; i++) {
        sche_lock();
    }
    MKRTOS_ASSERT(sche_lock_cn_get() == 10);
    for (int i = 0; i < 10; i++) {
        sche_unlock();
    }
    MKRTOS_ASSERT(sche_lock_cn_get() == 0);

    kprint("=======end sched mem\n");
    return 0;
}

INIT_TEST_REG(test_sched, INIT_TEST_LVL);
