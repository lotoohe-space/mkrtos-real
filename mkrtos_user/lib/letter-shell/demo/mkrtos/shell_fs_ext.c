#include "shell.h"
#include "cons_cli.h"
#include "pm_cli.h"
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/stat.h>
#include <errno.h>
#include "u_sys.h"
#include "unistd.h"
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
        struct stat st = {0};
        stat(ptr->d_name, &st);
        printf("%s\t\t\t%dB\n", ptr->d_name, st.st_size);
    }
    closedir(dir);
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), ls, ls, ls command);

int rm(int argc, char *agrv[])
{
    int ret;
    if (argc < 2)
    {
        return -1;
    }

    ret = unlink(agrv[1]);
    return ret;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), rm, rm, rm command);
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
        return errno;
    }

    while ((c = fgetc(fp)) != EOF)
    {
        cons_write((uint8_t *)&c, 1);
    }
    fclose(fp);

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), cat, cat, cat command);
int hex(int argc, char *argv[])
{
    int i = 0;
    if (argc != 2)
    {
        return (-1);
    }

    FILE *fp;
    char c;
    int ret;

    if ((fp = fopen(argv[1], "rb")) == NULL)
    {
        return errno;
    }

    while ((ret = fread(&c, 1, 1, fp)) == 1)
    {
        printf("%02x ", c);
        i++;
        if (i % 16 == 0)
        {
            printf("\n");
        }
    }
    printf("\nsize:%dB\n", i);
    fclose(fp);

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), hex, hex, hex command);
int kill(int argc, char *argv[])
{
    if (argc < 2)
    {
        return -1;
    }
    int pid = atoi(argv[1]);

    return pm_kill_task(pid, PM_KILL_TASK_ALL);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), kill, kill, kill command);

int shell_symlink(int argc, char *argv[])
{
    if (argc < 3)
    {
        return -1;
    }
    printf("%s %s %s\n", __func__, argv[1], argv[2]);
    return symlink(argv[1], argv[2]);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), symlink, shell_symlink, symlink command);

int shell_reboot(int argc, char *argv[])
{
    printf("sys reboot.\n");
    sys_reboot(SYS_PROT);
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), reboot, shell_reboot, reboot command);

int shell_mem_info(int argc, char *argv[])
{
    size_t total;
    size_t free;

    sys_mem_info(SYS_PROT, (umword_t *)&total, (umword_t *)&free);
    printf("sys mem:\ntotal:%dB\nfree:%dB\n", total, free);
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), mem, shell_mem_info, mem command);