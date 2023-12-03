#include "shell.h"
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
int ls(int argc, char *agrv[])
{
    DIR *dir;
    struct dirent *ptr;
    int i;
    if (argc < 2)
    {
        return -1;
    }

    dir = opendir(agrv[1]);
    if (!dir)
    {
        return 0;
    }
    while ((ptr = readdir(dir)) != NULL)
    {
        printf("%s \n", ptr->d_name);
    }
    closedir(dir);
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), ls, ls, ls command);
