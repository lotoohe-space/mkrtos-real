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
#include "cons_cli.h"
#include "u_sleep.h"
Shell shell;
char shellBuffer[512];

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
    int ret = 0;
    while (ret <= 0)
    {
        ret = cons_read((uint8_t *)data, len);
        u_sleep_ms(5);
    }
}

/**
 * @brief 用户shell初始化
 *
 */
void userShellInit(void)
{
    shell.write = userShellWrite;
    shell.read = userShellRead;
    shellInit(&shell, shellBuffer, 512);

    while (1)
    {
        shellTask(&shell);
    }
}
