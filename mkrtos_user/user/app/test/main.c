
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
int main(int argc, char *argv[])
{
    for (int i = 0; i < argc; i++)
    {
        printf("argv[%d]: %s\n", i, argv[i]);
    }
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
