/**
 * @file shell_port.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief
 * @version 0.1
 * @date 2019-02-22
 *
 * @copyright (c) 2019 Letter
 *
 */

#include "shell.h"
#include "shell_ext.h"
#include "shell_fs.h"
#include "shell_passthrough.h"
#include "shell_secure_user.h"
#include "log.h"
// #include "telnetd.h"
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include "cons_cli.h"
#include "u_sleep.h"
static Shell shell;
static ShellFs shellFs;
static char shellBuffer[256];
// static char shellPathBuffer[128] = "/";

/**
 * @brief 用户shell写
 *
 * @param data 数据
 * @param len 数据长度
 *
 * @return unsigned short 写入实际长度
 */
signed short userShellWrite(char *data, unsigned short len)
{
    return cons_write((const char *)data, len);
}

/**
 * @brief 用户shell读
 *
 * @param data 数据
 * @param len 数据长度
 *
 * @return unsigned short 读取实际长度
 */
signed short userShellRead(char *data, unsigned short len)
{
    while (cons_read((uint8_t *)data, len) <= 0)
    {
        u_sleep_ms(5);
    }
}
/**
 * @brief 列出文件
 *
 * @param path 路径
 * @param buffer 结果缓冲
 * @param maxLen 最大缓冲长度
 * @return size_t 0
 */
size_t userShellListDir(char *path, char *buffer, size_t maxLen)
{
    DIR *dir;
    struct dirent *ptr;
    int i;

    if (!path)
    {
        return 0;
    }
    dir = opendir(path);
    if (!dir)
    {
        return 0;
    }
    memset(buffer, 0, maxLen);
    while ((ptr = readdir(dir)) != NULL)
    {
        strcat(buffer, ptr->d_name);
        strcat(buffer, "\t");
    }
    closedir(dir);
    return 0;
}
/**
 * @brief 用户shell初始化
 *
 */
void userShellInit(void)
{
    // shellFs.getcwd = getcwd;
    // shellFs.chdir = chdir;
    // shellFs.listdir = userShellListDir;
    // shellFsInit(&shellFs, shellPathBuffer, sizeof(shellPathBuffer));
    shell.write = userShellWrite;
    shell.read = userShellRead;
    // shellSetPath(&shell, shellPathBuffer);
    shellInit(&shell, shellBuffer, sizeof(shellBuffer));
    // shellCompanionAdd(&shell, SHELL_COMPANION_ID_FS, &shellFs);

    while (1)
    {
        shellTask(&shell);
    }
}
