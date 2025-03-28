#include "cons_cli.h"
#include "pm_cli.h"
#include "shell.h"
#include "u_malloc.h"
#include "u_sys.h"
#include "unistd.h"
#include <dirent.h>
#include <errno.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <u_sleep.h>
int ls(int argc, char *agrv[])
{
    DIR *dir;
    struct dirent *ptr;
    int i;
    char *path;
    char *in_path;
    int ret;

    if (argc < 2)
    {
        in_path = "";
    }
    else
    {
        in_path = agrv[1];
    }

    path = u_malloc(PAGE_SIZE);
    if (path == NULL)
    {
        return -ENOMEM;
    }
    path = getcwd(path, PAGE_SIZE);
    if (path == NULL)
    {
        u_free(path);
        return ret;
    }
    ret = chdir(in_path);
    if (ret < 0)
    {
        u_free(path);
        return ret;
    }
    dir = opendir(".");
    if (!dir)
    {
        u_free(path);
        return 0;
    }
    printf("%-20s%10s\n", "name", "size");

    while ((ptr = readdir(dir)) != NULL)
    {
        struct stat st = {0};

        ret = stat(ptr->d_name, &st);
        printf("%-20s%10d\n", ptr->d_name, st.st_size);
    }
    closedir(dir);
    chdir(path);
    u_free(path);
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), ls, ls, ls command);
int cp(int argc, char *argv[])
{
#define BUFFER_SIZE 64

    int source_fd, target_fd;
    ssize_t bytes_read, bytes_written;
    char buffer[BUFFER_SIZE];
    int ret;

    // 检查参数数量
    if (argc != 3)
    {
        fprintf(stderr, "example: %s /bin/a.bin /mnt/a.bin\n", argv[0]);
        return -1;
    }

    // 打开源文件
    source_fd = open(argv[1], O_RDONLY);
    if (source_fd == -1)
    {
        perror("can not open src file.\n");
        return -1;
    }
    struct stat st = {0};

    ret = stat(argv[1], &st);

    // 打开目标文件（如果不存在则创建，如果存在则截断）
    target_fd = open(argv[2], O_WRONLY | O_CREAT, 0644);
    if (target_fd == -1)
    {
        perror("can not open dest file.");
        close(source_fd);
        return -1;
    }
    if (ftruncate(target_fd, st.st_size) < 0)
    {
        printf("to set fiel size is error.\n");
        return -1;
    }

    // 复制文件内容
    while ((bytes_read = read(source_fd, buffer, BUFFER_SIZE)) > 0)
    {
        bytes_written = write(target_fd, buffer, bytes_read);
        if (bytes_written != bytes_read)
        {
            perror("write file is error.\n");
            close(source_fd);
            close(target_fd);
            return -1;
        }
    }

    if (bytes_read == -1)
    {
        perror("read src file is error.\n");
    }

    // 关闭文件
    close(source_fd);
    close(target_fd);

    printf("cp file is success.\n");

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), cp, cp, cp command);
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
int cd(int argc, char *agrv[])
{
    int ret;
    if (argc < 2)
    {
        return -1;
    }

    ret = chdir(agrv[1]);
    if (ret >= 0)
    {
        setenv("PWD", agrv[1], 1);
    }
    return ret;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), cd, cd, cd command);
int pwd(int argc, char *agrv[])
{
    char *pwd;
    char path[64];

    pwd = getcwd(path, sizeof(path));
    printf("%s\n", pwd);
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), pwd, pwd, pwd command);
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
        write(STDOUT_FILENO, (uint8_t *)&c, 1);
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

    return pm_kill_task(pid, PM_KILL_TASK_ALL, 0);
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

int shell_touch(int argc, char *argv[])
{
    if (argc < 2)
    {
        return -1;
    }
    int fd;

    fd = open(argv[1], O_CREAT, 0777);
    if (fd < 0)
    {
        return fd;
    }
    close(fd);
    return fd;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), touch, shell_touch, touch command);
int shell_mkdir(int argc, char *argv[])
{
    if (argc < 2)
    {
        return -1;
    }
    int ret;

    ret = mkdir(argv[1], 0777);
    return ret;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), mkdir, shell_mkdir, mkdir command);
int shell_reboot(int argc, char *argv[])
{
    printf("sys reboot.\n");
    u_sys_reboot(SYS_PROT);
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), reboot, shell_reboot, reboot command);

int shell_mem_info(int argc, char *argv[])
{
    size_t total;
    size_t free;

    u_sys_mem_info(SYS_PROT, (umword_t *)&total, (umword_t *)&free);
    printf("sys mem:\ntotal:%dB\nfree:%dB\n", total, free);
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), free, shell_mem_info, free command);

int shell_sys_info(int argc, char *argv[])
{
    size_t total;
    size_t free;

    printf("sys:\n");
    printf("\tcpu usage:%d.%d\n", sys_read_cpu_usage() / 10, sys_read_cpu_usage() % 10);
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), sys, shell_sys_info, sys command);

int shell_exit(int argc, char *argv[])
{
    exit(0);
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), exit, shell_exit, exit command);
int shell_sleep(int argc, char *argv[])
{
    if (argc < 1)
    {
        return -1;
    }
    u_sleep_ms(1000 * atol(argv[1]));
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), sleep, shell_sleep, sleep command);