
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <shell_port.h>
#include <pm_cli.h>
#include <u_sleep.h>
#include <u_task.h>
int main(int argc, char *args[])
{
    task_set_obj_name(TASK_THIS, TASK_THIS, "tk_sh");
    task_set_obj_name(TASK_THIS, THREAD_MAIN, "th_sh");
#if 1
    thread_run(-1, 3);
#endif
    for (int i = 0; i < argc; i++)
    {
        printf("args[%d]:%s\n", i, args[i]);
    }
    userShellInit();
    return 0;
}
