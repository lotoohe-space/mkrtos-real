
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <shell_port.h>
#include <pm_cli.h>
int main(int argc, char *args[])
{
    for (int i = 0; i < argc; i++)
    {
        printf("args[%d]:%s\n", i, args[i]);
    }
    // pm_run_app("lcd_drv", 0);
    userShellInit();
    return 0;
}
