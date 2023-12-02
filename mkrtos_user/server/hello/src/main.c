
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
    uint8_t data[8];
    usleep(100000);

    printf("%s: please input str.\n", args[0]);
    scanf("%s", data);
    printf("%s\n", data);
    mr_drv_test();
    pm_test();

    return 0;
}
