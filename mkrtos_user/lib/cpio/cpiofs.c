
#include "u_types.h"
#include "cpiofs.h"
#include "u_util.h"
#include <string.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdio.h>
int htoi(char *str, int len)
{
    int n = 0;
    int i = 0;

    if (str == NULL || len <= 0)
        return -1;
    for (i = 0; i < len; i++)
    {
        if (str[i] != '0')
        {
            break;
        }
    }
    str += i;
    while (1)
    {
        if (*str >= '0' && *str <= '9')
        {
            n = 16 * n + (*str - '0');
        }
        else if (*str >= 'A' && *str <= 'F') // 十六进制还要判断字符是不是在A-F或者a-f之间
        {
            n = 16 * n + (*str - 'A' + 10);
        }
        else if (*str >= 'a' && *str <= 'f')
        {
            n = 16 * n + (*str - 'a' + 10);
        }
        else
        {
            break;
        }
        str++;
        i++;
        if (i >= len)
        {
            break;
        }
    }
    return n;
}
int cpio_find_next(umword_t st, const char *pre_name,
                   umword_t *size, int *type, umword_t *addr, const char **name)
{
    cpio_fs_t *file_info;
    uint32_t mode;
    assert(addr);
    assert(name);
    for (umword_t i = st;;)
    {
        file_info = (cpio_fs_t *)i;

        if (check_magic((char *)file_info) < 0)
        {
            return -1;
        }
        int name_size = htoi(file_info->c_namesize, 8);
        mode = htoi(file_info->c_mode, 8);
        const char *f_name = (char *)(i + sizeof(cpio_fs_t));
        if (strcmp("TRAILER!!", f_name) == 0)
        {
            return -1;
        }

        if (strncmp(f_name, pre_name, strlen(pre_name)) == 0)
        {
            if (S_ISDIR(mode))
            {
                *type = 1;
                *addr = (umword_t)(ALIGN(i + sizeof(cpio_fs_t) + name_size, 4));
            }
            else
            {
                *type = 0;
                int file_size = htoi(file_info->c_filesize, 8);

                *addr = ALIGN(ALIGN(i + sizeof(cpio_fs_t) + name_size, 4) +
                                  file_size,
                              4);
            }
            *name = f_name;
            return 0;
        }
        else
        {
            return -1;
        }
    }
    return -1;
}
int cpio_find_file(umword_t st, umword_t en, const char *name, umword_t *size, int *type, umword_t *addr)
{
    uint8_t rByte;
    int32_t bk_inx;
    cpio_fs_t *file_info;
    uint32_t mode;

    assert(addr);

    for (umword_t i = st; st < en;)
    {
        file_info = (cpio_fs_t *)i;

        if (check_magic((char *)file_info) < 0)
        {
            return -1;
        }
        int name_size = htoi(file_info->c_namesize, 8);
        mode = htoi(file_info->c_mode, 8);
        const char *f_name = (char *)(i + sizeof(cpio_fs_t));
        // printf("%s\n", f_name);
        if (name[0] == '\0')
        {
            if (type)
            {
                *type = 1;
            }
            *addr = (umword_t)(i);
            return 0;
        }
        if (strcmp(f_name, name) == 0)
        {
            if (S_ISDIR(mode))
            {
                if (type)
                {
                    *type = 1;
                }
                *addr = (umword_t)(ALIGN(i + sizeof(cpio_fs_t) + name_size, 4));
                return 0;
            }
            else
            {

                if (size)
                {
                    *size = htoi(file_info->c_filesize, 8);
                }
                if (type)
                {
                    *type = 0;
                }
                *addr = (umword_t)(ALIGN(i + sizeof(cpio_fs_t) + name_size, 4));
                return 0;
            }
        }
        if (strcmp("TRAILER!!", f_name) == 0)
        {
            return -1;
        }
        int file_size = htoi(file_info->c_filesize, 8);

        i = ALIGN(ALIGN(i + sizeof(cpio_fs_t) + name_size, 4) +
                      file_size,
                  4);
    }
    return -1;
}
