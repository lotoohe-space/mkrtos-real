/**
 * @file parse_cfg.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-11-27
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <u_types.h>
#include <u_prot.h>
#include <u_sys.h>
#include <errno.h>
#include <stdio.h>
#include <cpiofs.h>
#include <u_app_loader.h>
#include <u_env.h>
#include <string.h>
#include <sys/types.h>
#include <cons_svr.h>
#include <u_mutex.h>
#include <u_hd_man.h>
#include <stdlib.h>
#include "tty.h"
#include <parse_cfg.h>
#include <appfs_tiny.h>
static char cmd_line[CMD_LEN];                              //!< 命令行
static char cmd_params[CMD_PARAMS_CN][CMD_PARAMS_ITEM_LEN]; //!< 参数数组
static int cmd_params_off[CMD_PARAMS_CN];                   //!< 参数偏移量
static int cmd_params_num = 0;                              //!< 参数个数
static u_mutex_t cmd_lock;                                  //!< 命令行锁

#if CMD_PARAMS_CN < 1
#error "CMD_PARAMS_CN must be greater than 1"
#endif

/**
 * @brief 初始化命令行锁
 *
 */
void parse_cfg_init(void)
{
    u_mutex_init(&cmd_lock, handler_alloc());
}
/**
 * @brief   解析命令行
 * @return  int 应用使用的内存块
 */
static int parse_cfg_cmd_line(void)
{
    int cmd_params_inx = 0;
    int mem_block = 0;

    cmd_params_num = 0;
    printf("cmd_line:%s\n", cmd_line);
    for (int i = 0; i < sizeof(cmd_line) && cmd_line[i]; i++)
    {
        if (i == 0 && cmd_line[i] != ' ')
        {
            // 第一个参数
            cmd_params_off[cmd_params_inx++] = i;
            cmd_params_num++;
        }
        else if (cmd_line[i] == ' ')
        {
            // 下一个参数
            cmd_params_off[cmd_params_inx++] = i + 1;
            cmd_line[i] = 0;
            cmd_params_num++;
            if (cmd_params_inx >= CMD_PARAMS_CN)
            {
                // 参数个数超过最大值
                break;
            }
        }
        if (cmd_line[i] == '|')
        {
            cmd_line[i] = 0;
            mem_block = atol(&cmd_line[i + 1]); // 获取应用使用的内存块
            cmd_params_num--;
            break;
        }
    }
    return mem_block;
}

/**
 * @brief 查找行结束位置
 *
 * @param str 字符串
 * @param size 字符串长度
 * @return int 行结束位置
 */
static int find_line_end(const char *str, size_t size)
{
    for (int i = 0; i < size; i++)
    {
        if (str[i] == '\n')
        {
            return i + 1;
        }
    }
    return size;
}
/**
 * @brief 解析配置文件
 *
 * @param parse_cfg_file_name 配置文件名
 * @param env 环境变量
 * @return int 成功返回启动的应用个数，失败返回-1
 */
int parse_cfg(const char *parse_cfg_file_name, uenv_t *env)
{
    int ret = 0;
    int run_cn = 0;
    msg_tag_t tag;
    sys_info_t sys_info;

    u_mutex_lock(&cmd_lock, 0, NULL);

    tag = sys_read_info(SYS_PROT, &sys_info, 0);
    if (msg_tag_get_val(tag) < 0)
    {
        printf("read info is errno.\n");
        ret = -ENOENT;
        goto out;
    }
    umword_t size;
    int type;
    const char *str;
#if IS_ENABLED(CONFIG_CPIO_SUPPORT)
    ret = cpio_find_file((umword_t)sys_info.bootfs_start_addr,
                         (umword_t)(-1), parse_cfg_file_name, &size, &type, (umword_t *)&str);
#else
    str = (const char *)appfs_tiny_find_file_addr_by_name(appfs_tiny_get_form_addr((void *)sys_info.bootfs_start_addr), parse_cfg_file_name, &size);
    if (str == NULL)
    {
        ret = -ENOENT;
    }
    type = 0;
#endif
    if (ret < 0 || (ret >= 0 && type == 1))
    {
        ret = -ENOENT;
        goto out;
    }
    for (int i = 0; i < size;)
    {
        int line_inx = find_line_end(str + i, size - i);

        if (str[i] != '#')
        {
            if (strlen(&str[i]) != 0 && str[i] != '\n')
            {

                strncpy(cmd_line, &str[i], MIN(line_inx, sizeof(cmd_line)));
                if (line_inx + i >= size && str[i + line_inx - 1] != '\n')
                {
                    cmd_line[MIN(line_inx + 1, sizeof(cmd_line)) - 1] = 0;
                }
                else
                {
                    cmd_line[MIN(line_inx, sizeof(cmd_line)) - 1] = 0;
                }
                pid_t pid;
                int params_cn = 0;
                char *args[CMD_PARAMS_CN] = {
                    NULL,
                };
                int mem_block;

                mem_block = parse_cfg_cmd_line();
                for (int i = 0; i < cmd_params_num; i++)
                {
                    args[i] = &cmd_line[cmd_params_off[i]];
                    printf("parse_cfg args[%d] = %s\n", i, args[i]);
                }
                printf("parse_cfg cmd_params_num:%d\n", cmd_params_num);
                int ret = app_load(cmd_line, env, &pid, args, cmd_params_num,
                                   NULL, 0, mem_block);
                if (ret < 0)
                {
                    printf("%s load fail, 0x%x\n", cmd_line, ret);
                }
                else
                {
                    tty_set_fg_pid(pid);
                    run_cn++;
                }
            }
        }
        i += line_inx;
    }
out:
    if (ret < 0)
    {
        printf("parse cfg file fail, ret:%d\n", ret);
        return ret;
    }
    u_mutex_unlock(&cmd_lock);
    return run_cn;
}
