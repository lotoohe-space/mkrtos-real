
// #include <stdio.h>
#include "u_log.h"
#include "u_env.h"
#include "u_sleep.h"
#include "cons_cli.h"
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "test.h"
int main(int argc, char *args[])
{
    
    usleep(100000);

    printf("%s start running.\n", args[0]);

    mr_drv_test();
    pm_test();

    return 0;
}
