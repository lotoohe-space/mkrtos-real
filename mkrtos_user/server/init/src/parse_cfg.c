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
int parse_cfg(const char *parse_cfg_file_name, uenv_t *env)
{
    int run_cn = 0;
    msg_tag_t tag;
    sys_info_t sys_info;

    tag = sys_read_info(SYS_PROT, &sys_info);
    if (msg_tag_get_val(tag) < 0)
    {
        printf("read info is errno.\n");
        return -ENOENT;
    }
    umword_t size;
    int type;
    const char *str;
    int ret = cpio_find_file((umword_t)sys_info.bootfs_start_addr,
                             (umword_t)(-1), parse_cfg_file_name, &size, &type, (umword_t *)&str);

    if (ret < 0 || (ret >= 0 && type == 1))
    {
        return -ENOENT;
    }
    for (int i = 0; i < size;)
    {
        int line_inx = find_line_end(str + i, size - i);

        if (str[i] != '#')
        {
            if (strlen(&str[i]) != 0 && str[i] != '\n')
            {
                char name[32];

                strncpy(name, &str[i], MIN(line_inx, sizeof(name)));
                if (line_inx + i >= size && str[i + line_inx - 1] != '\n')
                {
                    name[MIN(line_inx + 1, sizeof(name)) - 1] = 0;
                }
                else
                {
                    name[MIN(line_inx, sizeof(name)) - 1] = 0;
                }
                pid_t pid;
                char *args[] = {
                    name,
                    "-t"
                };
                int ret = app_load(name, env, &pid, args, 2, NULL, 0);
                if (ret < 0)
                {
                    printf("%s load fail, 0x%x\n", name, ret);
                }
                else
                {
                    console_active(pid);
                    run_cn++;
                }
            }
        }
        i += line_inx;
    }

    return run_cn;
}
