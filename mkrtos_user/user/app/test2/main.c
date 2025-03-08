
#include "u_task.h"
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <u_sleep.h>
#include <u_vmam.h>
#include <unistd.h>
#include <stdlib.h>
int main(int argc, char *argv[])
{
    int fd;

    task_set_obj_name(TASK_THIS, TASK_THIS, "tk_tst2");
    task_set_obj_name(TASK_THIS, THREAD_MAIN, "th_tst2");
    fd = open("/mnt/1.txt", O_RDWR | O_CREAT, 0777);
    if (fd < 0) {
        printf("open faile:%d.\n", fd);
        return fd;
    }
    while(1){
        u_sleep_ms(100000);
    }
    exit(-3333);
    return 0;
}
