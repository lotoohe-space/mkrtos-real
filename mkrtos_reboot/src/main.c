#include <unistd.h>
#include <sys/reboot.h>
#include <stdlib.h>
#include <string.h>
// #include "str.h"

#define ABORTMSG "hard-reboot: Aborted.\n"
#define USAGE "Say \"hard-reboot (RESTART|HALT|POWER_OFF)\" if you really mean it.\n"

void usage(void)
{
    write(2, ABORTMSG, strlen(ABORTMSG));
    write(2, USAGE, strlen(USAGE));
    exit(1);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
        usage();

    sync();
    sync();
    sync();
    if (strcmp(argv[1], "RESTART") == 0)
    {
        reboot(RB_AUTOBOOT);
    }
    else if (strcmp(argv[1], "HALT") == 0)
    {
        reboot(RB_HALT_SYSTEM);
    }
    else if (strcmp(argv[1], "POWER_OFF") == 0)
    {
        reboot(RB_POWER_OFF);
    }
    else
    {
        usage();
    }

    while (1)
        sleep(10);
}
