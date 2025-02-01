
#include <assert.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <u_vmam.h>
#include <u_sleep.h>
extern int vi_main(int argc, char **argv);
int main(int argc, char *argv[])
{
    printf("%s running..\n", argv[0]);
    return vi_main(argc, argv);
}
