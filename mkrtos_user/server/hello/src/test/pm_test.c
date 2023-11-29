#include "pm_cli.h"
#include <stdio.h>

void pm_test(void)
{
    int ret = pm_run_app("rtthread_drv", 0);

    if (ret < 0)
    {
        printf("rtthread_drv start error.\n");
    }
}
