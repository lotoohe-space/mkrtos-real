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
#include <fcntl.h>
#include <sys/types.h>
#include <termios.h>
#include <sys/stat.h>
#include "cons_cli.h"
#include "u_sleep.h"
#include "u_sema.h"
#include "u_task.h"
#include <errno.h>
#include <stdlib.h>
static Shell shell;
static ShellFs shellFs;
static char shellBuffer[1024];
// static char shellPathBuffer[128] = "/";
static struct termios old_settings;
static struct termios new_settings;
int script_sh_fd = STDIN_FILENO;
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
    // if (script_sh_fd == STDIN_FILENO)
    {
        return write(STDOUT_FILENO, data, len);
    }
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
    int rlen;

    rlen = read(script_sh_fd, data, len);
    if (rlen <= 0)
    {
        if (errno != ENOTTY && errno != EAGAIN) {
            exit(0);
        }
    }
    return rlen;
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

void tty_set_raw_mode(void)
{
    if (script_sh_fd == STDIN_FILENO)
    {
        new_settings = old_settings;
        new_settings.c_lflag &= ~(ICANON | ECHO);   // 禁用规范模式和回显
        new_settings.c_cc[VMIN] = 1;                // 读取的最小字符数
        new_settings.c_cc[VTIME] = 0;               // 读取的超时时间（以10ms为单位）
        tcsetattr(script_sh_fd, TCSANOW, &new_settings);
    }
}
void tty_set_normal_mode(void)
{
    if (script_sh_fd == STDIN_FILENO)
    {
        tcsetattr(script_sh_fd, TCSANOW, &old_settings);
    }
}
/**
 * @brief 用户shell初始化
 *
 */
void userShellInit(int argc, char *args[])
{
    if (argc > 1)
    {
        printf("run script:%s\n", args[1]);
        script_sh_fd = open(args[1], O_RDWR, 0777);
        if (script_sh_fd < 0)
        {
            script_sh_fd = STDIN_FILENO;
        }
    } else {
        script_sh_fd = STDIN_FILENO;
    }
    if (script_sh_fd == STDIN_FILENO)
    {
        tcgetattr(script_sh_fd, &old_settings);
        tty_set_raw_mode();
    }
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
