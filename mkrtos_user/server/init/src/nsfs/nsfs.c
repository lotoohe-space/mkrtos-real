#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include "ns.h"

#define DIR_INFO_CACHE_NR 32

typedef struct dir_info_cache
{
    ns_node_t *info;
    int ref;
} dir_info_cache_t;

static dir_info_cache_t dir_info_cache_list[DIR_INFO_CACHE_NR];
static int dir_info_cache_get(ns_node_t *info)
{
    if (info == NULL)
    {
        return -EINVAL;
    }
    for (int i = 0; i < DIR_INFO_CACHE_NR; i++)
    {
        if (dir_info_cache_list[i].info == NULL)
        {
            continue;
        }
        if (strcmp(dir_info_cache_list[i].info->name, info->name) == 0)
        {
            dir_info_cache_list[i].ref++;
            return i;
        }
    }
    for (int i = 0; i < DIR_INFO_CACHE_NR; i++)
    {
        if (dir_info_cache_list[i].info == NULL)
        {
            dir_info_cache_list[i].info = info;
            dir_info_cache_list[i].ref++;
            return i;
        }
    }
    return -1;
}
static void dir_info_cache_put(ns_node_t *info)
{
    assert(info);
    for (int i = 0; i < DIR_INFO_CACHE_NR; i++)
    {
        if (dir_info_cache_list[i].info == NULL)
        {
            continue;
        }
        if (strcmp(dir_info_cache_list[i].info->name, info->name) == 0)
        {
            dir_info_cache_list[i].ref--;
            if (dir_info_cache_list[i].ref == 0)
            {
                dir_info_cache_list[i].info = NULL;
                return;
            }
        }
    }
}
enum fs_ns_type
{
    FS_NS_FILE_TYPE,
    FS_NS_DIR_TYPE,
};
#define DIR_INFO_NR 32
typedef struct fs_ns_file
{
    int dir_info_fd;      //!< dirinfo cache fd.
    int tmp_fd;           //!< current info fd.
    uint8_t flags;        //!< 文件操作的flags
    int offset;           //!< 文件操作的偏移
    enum fs_ns_type type; //!< 类型
    uint8_t used;
} fs_ns_file_t;

static fs_ns_file_t fs_ns_files[DIR_INFO_NR];

static fs_ns_file_t *fs_ns_get_file(int fd);

static int fs_ns_file_alloc(ns_node_t *file, int flags, enum fs_ns_type type)
{
    int dir_info_fd = -1;

    if (file != NULL)
    {
        dir_info_fd = dir_info_cache_get(file);
        if (dir_info_fd < 0)
        {
            printf("dir_info cache is full.\n");
            return dir_info_fd;
        }
    }

    for (int i = 0; i < DIR_INFO_NR; i++)
    {
        if (fs_ns_files[i].used == 0)
        {
            // 不存在则增加一个新的
            fs_ns_files[i].dir_info_fd = dir_info_fd;
            fs_ns_files[i].tmp_fd = -1;
            fs_ns_files[i].flags = flags;
            fs_ns_files[i].offset = 0;
            fs_ns_files[i].type = type;
            fs_ns_files[i].used = 1;
            return i;
        }
    }
    return -1;
}

static fs_ns_file_t *fs_ns_get_file(int fd)
{
    if (fd < 0 || fd >= DIR_INFO_NR)
    {
        return NULL;
    }
    if (fs_ns_files[fd].used == 0)
    {
        return NULL;
    }
    return fs_ns_files + fd;
}

int fs_ns_open(const char *name, int flags, int mode)
{
    int ret;
    ns_node_t *file;
    int fd;
    int cur_inx;
    enum fs_ns_type type;

    // 路径必须'/'卡头
    if (name[0] == '\0' || name[0] != '/')
    {
        printf("nsfs path is error.\n");
        return -EINVAL;
    }
    if (flags & O_CREAT)
    {
        // not support to create file.
        return -ENOSYS;
    }
    // path must is dir.
    file = ns_node_find_full_dir(name, &ret, &cur_inx);
    if (file == NULL)
    {
        return -EINVAL;
    }
    type = FS_NS_DIR_TYPE;
    fd = fs_ns_file_alloc(file, flags, type);
    if (fd < 0)
    {
        return fd;
    }
    return fd;
}
int fs_ns_write(int fd, void *data, int len)
{
    return -ENOSYS;
}
int fs_ns_read(int fd, void *data, int len)
{
    return -ENOSYS;
}

int fs_ns_ioctl(int fd, unsigned long cmd, unsigned long arg)
{
    return -ENOSYS;
}
static int fs_ns_get_dir_size(fs_ns_file_t *file)
{
    return ns_nodes_count(dir_info_cache_list[file->dir_info_fd].info);
}
int fs_ns_lseek(int fd, int offset, unsigned long whence)
{
    fs_ns_file_t *file = fs_ns_get_file(fd);
    int new_offs = 0;
    int old_offs = 0;

    if (!file || file->used == 0)
    {
        return -ENOENT;
    }
    old_offs = file->offset;
    switch (whence)
    {
    case SEEK_SET:
        new_offs = offset;
        break;
    case SEEK_END:
    {
        new_offs = fs_ns_get_dir_size(file) + offset;
    }
    break;
    case SEEK_CUR:
    {
        new_offs = offset + file->offset;
    }
    break;
    default:
        return -EINVAL;
    }
    if (new_offs > fs_ns_get_dir_size(file))
    {
        new_offs = fs_ns_get_dir_size(file);
    }
    if (new_offs < 0)
    {
        new_offs = 0;
    }
    if (file->type == FS_NS_DIR_TYPE)
    {
        ns_node_t *new_dir_info;

        new_dir_info = ns_node_get_inx(dir_info_cache_list[file->dir_info_fd].info, new_offs);
        if (new_dir_info == NULL)
        {
            return -ENOENT;
        }
        if (file->tmp_fd >= 0)
        {
            dir_info_cache_put(dir_info_cache_list[file->tmp_fd].info);
        }
        file->tmp_fd = dir_info_cache_get(new_dir_info);
    }
    file->offset = new_offs;

    return old_offs;
}
int fs_ns_truncate(int fd, off_t length)
{
    return -ENOSYS;
}
int fs_ns_stat(int fd, struct stat *st)
{
    fs_ns_file_t *file = fs_ns_get_file(fd);

    if (!file || file->used == 0)
    {
        return -ENOENT;
    }
    memset(st, 0, sizeof(*st));
    st->st_size = 0;
    st->st_mode = file->type == FS_NS_FILE_TYPE ? S_IFREG : S_IFDIR;
    st->st_nlink = dir_info_cache_list[file->dir_info_fd].info->ref;
    return 0;
}
int fs_ns_close(int fd)
{
    fs_ns_file_t *file = fs_ns_get_file(fd);
    if (!file || file->used == 0)
    {
        return -ENOENT;
    }
    if (file->tmp_fd >= 0)
    {
        dir_info_cache_put(dir_info_cache_list[file->tmp_fd].info);
    }
    dir_info_cache_put(dir_info_cache_list[file->dir_info_fd].info);
    file->dir_info_fd = -1;
    file->tmp_fd = -1;
    file->flags = 0;
    file->offset = 0;
    file->used = 0;
    file->type = FS_NS_FILE_TYPE;
    return 0;
}
int fs_ns_rmdir(const char *name)
{
    ns_node_t *file;
    int ret;
    int dir_info_fd;
    int cur_inx;

    file = ns_node_find_full_dir(name, &ret, &cur_inx);
    if (file == NULL)
    {
        return -ENOENT;
    }

    dir_info_fd = dir_info_cache_get(file);
    if (dir_info_cache_list[dir_info_fd].ref == 1)
    {
        ret = ns_delnode(name);
        if (ret < 0)
        {
            dir_info_cache_put(file);
            return ret;
        }
    }
    dir_info_cache_put(file);
    return 0;
}
int fs_ns_remove(const char *name)
{
    ns_node_t *file;
    int ret;
    int dir_info_fd;
    int cur_inx;

    file = ns_node_find_full_file(name, &ret, &cur_inx);
    if (file == NULL)
    {
        return -ENOENT;
    }

    dir_info_fd = dir_info_cache_get(file);
    if (dir_info_cache_list[dir_info_fd].ref == 1)
    {
        ret = ns_delnode(name);
        if (ret < 0)
        {
            return ret;
        }
    }
    dir_info_cache_put(file);
    return 0;
}
int fs_ns_readdir(int fd, struct dirent *_dir)
{
    int ret = -1;
    fs_ns_file_t *file = fs_ns_get_file(fd);

    if (!file || file->used == 0)
    {
        return -ENOENT;
    }
    if (file->type != FS_NS_DIR_TYPE)
    {
        return -EACCES;
    }
    if (file->tmp_fd == -1)
    {
        ns_node_t *dir_info;

        dir_info = ns_node_get_first(dir_info_cache_list[file->dir_info_fd].info);
        if (dir_info == NULL)
        {
            return -ENOENT;
        }
        ret = dir_info_cache_get(dir_info);
        if (ret < 0)
        {
            return ret;
        }
        file->tmp_fd = ret;
    }
    else if (file->tmp_fd == -2)
    {
        return -ENOENT;
    }
    ns_node_t *node_info;

    node_info = dir_info_cache_list[file->tmp_fd].info;
    _dir->d_reclen = sizeof(*_dir);
    _dir->d_type = node_info->type == NODE_TYPE_DUMMY ? DT_DIR : DT_REG; // DT_REG‌;
    strncpy(_dir->d_name, node_info->name, sizeof(_dir->d_name));
    _dir->d_name[sizeof(_dir->d_name) - 1] = 0;

    ns_node_t *next_dir_info = ns_node_get_next(dir_info_cache_list[file->dir_info_fd].info, node_info);
    dir_info_cache_put(node_info);
    ret = dir_info_cache_get(next_dir_info);
    if (ret >= 0)
    {
        file->tmp_fd = ret;
        file->offset++;
    }
    else
    {
        file->tmp_fd = -2; // -2代表结束遍历
    }
    return sizeof(*_dir);
}
int fs_ns_mkdir(char *path)
{
    int ret;
    ret = ns_mknode(path, HANDLER_INVALID, NODE_TYPE_DUMMY);
    return ret;
}
int fs_ns_open_init(void)
{
    for (int i = 0; i < DIR_INFO_NR; i++)
    {
        fs_ns_files[i].dir_info_fd = -1;
        fs_ns_files[i].tmp_fd = -1;
    }
    return 0;
}
