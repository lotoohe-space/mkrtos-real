#pragma once
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
struct hw_callback;
typedef struct hw_callback hw_callback_t;
struct fs_info;
typedef struct fs_info fs_info_t;

#ifndef ROUND_UP
#define ROUND(a, b) (((a) / (b)) + (((a) % (b)) ? 1 : 0)) //!< a/b后的值向上取整
#define ROUND_UP(a, b) ROUND(a, b)                        //!< a除b向上取整数
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b)) //!< 最小值
#endif

#ifndef ROUND_DOWN
#define ROUND_DOWN(a, b) ((a) / (b)) //!< a/b向下取整
#endif

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

typedef struct hw_callback
{
    int (*hw_erase_block)(fs_info_t *fs, int block_inx);                                //!< 擦除块
    int (*hw_write_and_erase_block)(fs_info_t *fs, int block_inx, void *buf, int size); //!< 擦除并写入
    int (*hw_read_block)(fs_info_t *fs, int block_inx, void *buf, int size);            //!< 读块
    int (*hw_init_fs_for_block)(fs_info_t *fs);                                         //!< 初始化
} hw_callback_t;

typedef struct fs_info
{
    fs_info_save_t save;    //!< 保存的文件系统信息
    uint32_t *buf;          //!< 缓冲区
    unsigned long mem_addr; //!< 内存地址
    int dev_fd;             //!< device file descriptor
    hw_callback_t cb;
} fs_info_t;

#define APPFS_FILE_NAME_MAX 52 //!< 文件名最大长度

typedef struct dir_info
{
    char name[APPFS_FILE_NAME_MAX]; //!< 文件名
    int block_inx;                  //!< 文件起始块号
    int size;                       //!< 文件大小
    int ref;                        //!< 引用计数
} dir_info_t;

int appfs_init(fs_info_t *fs);
int appfs_format(fs_info_t *fs, int file_nr);
int appfs_create_file(fs_info_t *info, const char *path, int size);
int appfs_write_file(fs_info_t *info, const char *name, void *data, int size, int offset);
int appfs_read_file(fs_info_t *info, const char *name, void *data, int size, int offset);
int appfs_delete_file(fs_info_t *info, const char *name);
int appfs_file_resize_raw(fs_info_t *info, const dir_info_t *dir_info, int size);
int appfs_file_resize(fs_info_t *info, const char *name, int size);
int appfs_arrange_files(fs_info_t *info);
int appfs_get_available_size(fs_info_t *info);
int appfs_print_files(fs_info_t *info);
static inline int appfs_get_total_size(fs_info_t *info)
{
    return info->save.block_nr * info->save.block_size;
}
static inline int appfs_get_file_size(const dir_info_t *info)
{
    return info->size;
}
static inline unsigned long appfs_get_file_addr(fs_info_t *fs, const dir_info_t *info)
{
    return info->block_inx * fs->save.block_size + fs->mem_addr; //!< 计算文件地址
}

const dir_info_t *appfs_find_file_by_name(fs_info_t *info, const char *name);
const dir_info_t *appfs_dir_info_get_next(fs_info_t *info, const dir_info_t *cur);
const dir_info_t *appfs_dir_info_get_first(fs_info_t *info);
const dir_info_t *appfs_dir_info_get_index(fs_info_t *info, int index);
const int appfs_get_dir_info_size(fs_info_t *info);

static inline fs_info_t *appfs_get_form_addr(void *addr)
{
    assert(addr);
    fs_info_t *fs = addr;
    const char *magic_str = APPFS_MAGIC;

    for (int i = 0; magic_str[i]; i++)
    {
        if (fs->save.magic[0] != magic_str[0])
        {
            return NULL;
        }
    }
    return fs;
}
static inline void *appfs_find_file_addr_by_name(fs_info_t *info, const char *name, size_t *size)
{
    const dir_info_t *dir_info;

    dir_info = appfs_find_file_by_name(info, name);

    if (dir_info == NULL)
    {
        return NULL;
    }
    if (size)
    {
        *size = dir_info->size;
    }
    return (void *)appfs_get_file_addr(info, dir_info);
}
