
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <shell_port.h>
#include <pm_cli.h>
#include <u_sleep.h>
int main(int argc, char *args[])
{
    task_set_obj_name(TASK_THIS, TASK_THIS, "tk_sh");
    task_set_obj_name(TASK_THIS, THREAD_MAIN, "th_sh");
    // u_sleep_ms(100);
    for (int i = 0; i < argc; i++)
    {
        printf("args[%d]:%s\n ", i, args[i]);
    }
    // pm_run_app("lcd_drv", 0);
    userShellInit();
    return 0;
}
