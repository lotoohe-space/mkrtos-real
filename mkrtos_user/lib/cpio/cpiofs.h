#pragma once
#include "u_types.h"
//! 26bytes
#pragma pack(1)
typedef struct cpio_fs
{
    char c_magic[6];
    char c_ino[8];
    char c_mode[8];
    char c_uid[8];
    char c_gid[8];
    char c_nlink[8];
    char c_mtime[8];
    char c_filesize[8];
    char c_devmajor[8];
    char c_devminor[8];
    char c_rdevmajor[8];
    char c_rdevminor[8];
    char c_namesize[8];
    char c_check[8];
} cpio_fs_t;

static inline int64_t check_magic(char magic[6])
{
    if (magic[0] == '0' && magic[1] == '7' && magic[2] == '0' &&
        magic[3] == '7' && magic[4] == '0' && magic[5] == '1')
    {
        return 0;
    }
    return -1;
}

int htoi(char *str, int len);
umword_t cpio_find_file(umword_t st, umword_t en, const char *name, umword_t *size);
