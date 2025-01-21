#pragma once

#include "types.h"
struct hw_callback;
typedef struct hw_callback hw_callback_t;
struct fs_info;
typedef struct fs_info fs_info_t;
#define APPFS_MAGIC "APPFS"
#define APPFS_VERSION 1

typedef struct fs_info_save
{
    char magic[8];     //!< APPFS
    int version;       //!< 版本号
    int blockinfo_inx; //!< 块信息起始块号
    int blockinfo_nr;  //!< 块信息块数量

    int dirinfo_inx; //!< 目录信息起始块号
    int dirinfo_nr;  //!< 目录信息块数量

    int block_inx; //!< 块起始块号
    int block_nr;  //!< 块数量

    int block_size; //!< 块大小
} fs_info_save_t;

typedef struct fs_info
{
    fs_info_save_t save; //!< 保存的文件系统信息
} fs_info_t;

#define APPFS_FILE_NAME_MAX 52 //!< 文件名最大长度

typedef struct dir_info
{
    char name[APPFS_FILE_NAME_MAX]; //!< 文件名
    int block_inx;                  //!< 文件起始块号
    int size;                       //!< 文件大小
    int ref;                        //!< 引用计数
} dir_info_t;
static inline unsigned long appfs_get_file_addr(fs_info_t *fs, const dir_info_t *info)
{
    assert(fs);
    return info->block_inx * fs->save.block_size + (unsigned long)fs; //!< 计算文件地址
}

fs_info_t *appfs_get_form_addr(void *addr);
const dir_info_t *appfs_find_file_by_name(fs_info_t *info, const char *name);
umword_t appfs_find_file_addr_by_name(fs_info_t *info, const char *name, size_t *size);
