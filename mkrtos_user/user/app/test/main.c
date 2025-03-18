
#include <assert.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <u_vmam.h>
#include "fs_test.h"
#include "mm_test.h"
#include "net_test.h"
#include "shm_test.h"
#include "u_task.h"
#include <u_sleep.h> 
int main(int argc, char *argv[])
{
    for (int i = 0; i < argc; i++)
    {
        printf("argv[%d]: %s\n", i, argv[i]);
    }
    posix_shem_test();
    system_v_shm_test();
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
