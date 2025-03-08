
#include <assert.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <u_vmam.h>
#include "fs_test.h"
#include "mm_test.h"
#include <u_sleep.h>
#include "net_test.h"
#include "u_task.h"
int main(int argc, char *argv[])
{
    task_set_obj_name(TASK_THIS, TASK_THIS, "tk_tst");
    task_set_obj_name(TASK_THIS, THREAD_MAIN, "th_tst");
    for (int i = 0; i < argc; i++)
    {
        printf("argv[%d]: %s\n", i, argv[i]);
    }
    exit(-1);
    net_test();
    fs_test3();
    fs_test2();
    fs_test();
    mm_test();
    mm_test1();
    // while (1)
    // {
    //     u_sleep_ms(1000);
    // }
    return 0;
}
