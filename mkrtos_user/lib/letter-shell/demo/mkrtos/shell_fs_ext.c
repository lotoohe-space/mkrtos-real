#include "shell.h"
#include "cons_cli.h"

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
int cat(int argc, char *argv[])
{
    if (argc != 2)
    {
        return (-1);
    }

    FILE *fp;
    int c;

    if ((fp = fopen(argv[1], "r")) == NULL)
    {
        return (-2);
    }

    while ((c = fgetc(fp)) != EOF)
    {
        cons_write(&c, 1);
    }
    cons_write_str("\n");
    fclose(fp);

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), cat, cat, cat command);

