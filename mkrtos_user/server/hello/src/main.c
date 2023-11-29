
// #include <stdio.h>
#include "u_log.h"
#include "u_env.h"
#include "u_sleep.h"
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include "test.h"
int main(int argc, char *args[])
{
    printf("%s start running.\n", args[0]);
    usleep(100000);

    ulog_write_str(u_get_global_env()->log_hd, "test start...\n");
    mr_drv_test();
    pm_test();
    ulog_write_str(u_get_global_env()->log_hd, "test end.\n");

    return 0;
}
