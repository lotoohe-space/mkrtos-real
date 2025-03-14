#include "appfs_open.h"
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
#include "appfs.h"
#ifdef MKRTOS
#include <u_thread.h>
#endif
static fs_info_t *fs;

#define DIR_INFO_CACHE_NR 32
typedef struct dir_info_cache
{
    const dir_info_t *info;
    int ref;
} dir_info_cache_t;

static dir_info_cache_t dir_info_cache_list[DIR_INFO_CACHE_NR];
static int dir_info_cache_get(const dir_info_t *info)
{
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
static void dir_info_cache_put(const dir_info_t *info)
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
enum appfs_type
{
    APPFS_FILE_TYPE,
    APPFS_DIR_TYPE,
};
#define DIR_INFO_NR 32
typedef struct appfs_file
{
    int dir_info_fd;      //!< dirinfo cache fd.
    uint8_t flags;        //!< 文件操作的flags
    int offset;           //!< 文件操作的偏移
    enum appfs_type type; //!< 类型
    uint8_t used;         //!< 使用中
    int pid;              //!< 属于哪个进程
} appfs_file_t;

static appfs_file_t appfs_files[DIR_INFO_NR];

static appfs_file_t *appfs_get_file(int fd);

void appfs_task_free(int pid)
{
    for (int i = 0; i < DIR_INFO_NR; i++)
    {
        if (appfs_files[i].dir_info_fd != -1)
        {
            appfs_close(i);
            printf("free fd:%d\n", i);
        }
    }
}

static int appfs_file_alloc(const dir_info_t *file, int flags, enum appfs_type type, int pid)
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
        if (appfs_files[i].used == 0)
        {
            // 不存在则增加一个新的
            appfs_files[i].dir_info_fd = dir_info_fd;
            appfs_files[i].flags = flags;
            appfs_files[i].pid = pid;
            appfs_files[i].offset = 0;
            appfs_files[i].type = type;
            appfs_files[i].used = 1;
            return i;
        }
    }
    return -1;
}

static appfs_file_t *appfs_get_file(int fd)
{
    if (fd < 0 || fd >= DIR_INFO_NR)
    {
        return NULL;
    }
    if (appfs_files[fd].used == 0)
    {
        return NULL;
    }
    return appfs_files + fd;
}

int appfs_open(const char *name, int flags, int mode)
{
#ifdef MKRTOS
    int pid = thread_get_src_pid();
#else
    int pid = 0;
#endif
    int ret;
    const dir_info_t *file;
    int fd;
    enum appfs_type type;

    // appfs只有根目录一级，所以打开目录只会有根目录
    if (name[0] == '\0' || (name[0] == '/' && name[1] == '\0'))
    {
        type = APPFS_DIR_TYPE;
    }
    else
    {
        if (name[0] == '/')
        {
            name++;
        }
        type = APPFS_FILE_TYPE;
    }
    if (type == APPFS_FILE_TYPE)
    {
        if (flags & O_CREAT)
        {
            ret = appfs_create_file(fs, name, 0);
        }
        if (ret < 0)
        {
            if (ret != -EEXIST)
            {
                return ret;
            }
        }
        file = appfs_find_file_by_name(fs, name);
        if (file == NULL)
        {
            return -ENOENT;
        }
        fd = appfs_file_alloc(file, flags, type, pid);
        if (fd < 0)
        {
            return fd;
        }
    }
    else if (type == APPFS_DIR_TYPE)
    {
        fd = appfs_file_alloc(NULL, flags, type, pid);
        if (fd < 0)
        {
            return fd;
        }
    }
    return fd;
}
int appfs_write(int fd, void *data, int len)
{
    appfs_file_t *file = appfs_get_file(fd);

    if (!file || file->used == 0)
    {
        return -ENOENT;
    }
    if (file->type != APPFS_FILE_TYPE)
    {
        return -EACCES;
    }
    int ret;

    ret = appfs_write_file(fs, dir_info_cache_list[file->dir_info_fd].info->name, data, len, file->offset);
    if (ret < 0)
    {
        return ret;
    }
    file->offset += ret;
    return ret;
}
int appfs_read(int fd, void *data, int len)
{
    appfs_file_t *file = appfs_get_file(fd);

    if (!file || file->used == 0)
    {
        return -ENOENT;
    }
    if (file->type != APPFS_FILE_TYPE)
    {
        return -EACCES;
    }
    int ret;

    ret = appfs_read_file(fs, dir_info_cache_list[file->dir_info_fd].info->name, data, len, file->offset);
    if (ret < 0)
    {
        return ret;
    }
    file->offset += ret;
    return ret;
}
#ifdef MKRTOS
#include <u_prot.h>
#include <u_task.h>
#include <u_thread.h>
#include <mk_access.h>
#include "kstat.h"
#endif
int appfs_ioctl(int fd, unsigned long cmd, unsigned long arg)
{
#ifdef MKRTOS
    int src_pid = thread_get_src_pid();
#endif
    appfs_file_t *file = appfs_get_file(fd);
    int ret = 0;

    if (!file || file->used == 0)
    {
        return -ENOENT;
    }
    if (file->type != APPFS_FILE_TYPE)
    {
        return -EACCES;
    }
    switch (cmd)
    {
    case APPFS_IOCTOL_GET_ACCESS_ADDR:
    {
        appfs_ioctl_arg_t *fs_arg;

#ifdef MKRTOS
        msg_tag_t tag;
        appfs_ioctl_arg_t fs_arg_tmp;
        umword_t cur_pid;
        tag = task_get_pid(TASK_THIS, (umword_t *)(&cur_pid));
        if (msg_tag_get_val(tag) < 0)
        {
            return msg_tag_get_val(tag);
        }
        fs_arg = &fs_arg_tmp;
#else
        fs_arg = (void *)arg;
#endif

        fs_arg->addr = appfs_get_file_addr(fs, dir_info_cache_list[file->dir_info_fd].info);
        fs_arg->size = appfs_get_file_size(dir_info_cache_list[file->dir_info_fd].info);
#ifdef MKRTOS
        ret = mk_copy_mem_to_task(cur_pid, fs_arg, src_pid,
                                  (void *)arg, sizeof(appfs_ioctl_arg_t));
        if (ret < 0)
        {
            return ret;
        }
#endif
    }
    break;
    default:
        ret = -ENOSYS;
        break;
    }
    return ret;
}
static int appfs_get_file_dir_size(appfs_file_t *file)
{
    switch (file->type)
    {
    case APPFS_DIR_TYPE:
        return appfs_get_file_size(dir_info_cache_list[file->dir_info_fd].info);
    case APPFS_FILE_TYPE:
        return appfs_get_dir_info_size(fs);
    default:
        assert(0);
    }
    return -1;
}
int appfs_lseek(int fd, int offset, unsigned long whence)
{
    appfs_file_t *file = appfs_get_file(fd);
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
        new_offs = appfs_get_file_dir_size(file) + offset;
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
    if (new_offs > appfs_get_file_dir_size(file))
    {
        if (file->type == APPFS_FILE_TYPE)
        {
            int ret;

            ret = appfs_file_resize_raw(fs, dir_info_cache_list[file->dir_info_fd].info, new_offs);
            if (ret < 0)
            {
                return ret;
            }
        }
        else
        {
            new_offs = appfs_get_file_dir_size(file);
        }
    }
    if (new_offs < 0)
    {
        new_offs = 0;
    }
    if (file->type == APPFS_DIR_TYPE)
    {
        const dir_info_t *new_dir_info;

        new_dir_info = appfs_dir_info_get_index(fs, new_offs);
        if (new_dir_info == NULL)
        {
            return -ENOENT;
        }
        if (file->dir_info_fd != -1)
        {
            dir_info_cache_put(dir_info_cache_list[file->dir_info_fd].info);
        }
        dir_info_cache_get(new_dir_info);
    }
    file->offset = new_offs;

    return old_offs;
}
int appfs_truncate(int fd, off_t length)
{
    appfs_file_t *file = appfs_get_file(fd);
    int ret;

    if (!file || file->used == 0)
    {
        return -ENOENT;
    }
    if (file->type != APPFS_FILE_TYPE)
    {
        return -EACCES;
    }
    ret = appfs_file_resize_raw(fs, dir_info_cache_list[file->dir_info_fd].info, length);
    return ret;
}
int appfs_fstat(int fd, struct kstat *st)
{
    appfs_file_t *file = appfs_get_file(fd);

    if (!file || file->used == 0)
    {
        return -ENOENT;
    }
    if (file->type != APPFS_FILE_TYPE)
    {
        return -EACCES;
    }
    st->st_size = dir_info_cache_list[file->dir_info_fd].info->size;
    st->st_mode = S_IFREG;
    st->st_nlink = dir_info_cache_list[file->dir_info_fd].info->ref;
    return 0;
}
int appfs_stat(const char *path, struct kstat *st)
{
    const dir_info_t *file;

    if (path[0] == '\0' || (path[0] == '/' && path[1] == '\0'))
    {
        st->st_size = 0;
        st->st_mode = S_IFDIR;
        st->st_nlink = 0;
        return 0;
    }
    if (path[0] == '/')
    {
        path++;
    }
    file = appfs_find_file_by_name(fs, path);
    if (file == NULL)
    {
        return -ENOENT;
    }
    st->st_size = file->size;
    st->st_mode = S_IFREG;
    st->st_nlink = file->ref;
    return 0;
}

int appfs_close(int fd)
{
    appfs_file_t *file = appfs_get_file(fd);
    if (!file || file->used == 0)
    {
        return -ENOENT;
    }
    if (file->type == APPFS_FILE_TYPE)
    {
        dir_info_cache_put(dir_info_cache_list[file->dir_info_fd].info);
    }
    else
    {
        if (file->dir_info_fd != -1)
        {
            dir_info_cache_put(dir_info_cache_list[file->dir_info_fd].info);
        }
    }
    file->dir_info_fd = -1;
    file->pid = -1;
    file->flags = 0;
    file->offset = 0;
    file->used = 0;
    file->type = APPFS_FILE_TYPE;
    return 0;
}
int appfs_remove(const char *name)
{
    const dir_info_t *file;
    int ret;
    int dir_info_fd;

    file = appfs_find_file_by_name(fs, name);
    if (file == NULL)
    {
        return -ENOENT;
    }

    dir_info_fd = dir_info_cache_get(file);
    if (dir_info_cache_list[dir_info_fd].ref == 1)
    {
        // 只有ref为1才能删除
        ret = appfs_delete_file(fs, name);
        if (ret < 0)
        {
            return ret;
        }
    }
    dir_info_cache_put(file);
    return 0;
}
int appfs_readdir(int fd, struct dirent *_dir)
{
    int ret = -1;
    // *((char*)(0)) = 0;
    appfs_file_t *file = appfs_get_file(fd);

    if (!file || file->used == 0)
    {
        return -ENOENT;
    }
    if (file->type != APPFS_DIR_TYPE)
    {
        return -EACCES;
    }
    if (file->dir_info_fd == -1)
    {
        const dir_info_t *dir_info = appfs_dir_info_get_first(fs);

        ret = dir_info_cache_get(dir_info);
        if (ret < 0)
        {
            return ret;
        }
        file->dir_info_fd = ret;
    }
    _dir->d_reclen = sizeof(*_dir);
    _dir->d_type = 4; // DT_REG‌;
    strncpy(_dir->d_name, dir_info_cache_list[file->dir_info_fd].info->name, sizeof(_dir->d_name));
    _dir->d_name[sizeof(_dir->d_name) - 1] = 0;

    const dir_info_t *next_dir_info =
        appfs_dir_info_get_next(fs, dir_info_cache_list[file->dir_info_fd].info);
    if (next_dir_info == NULL)
    {
        return -ENOENT;
    }
    dir_info_cache_put(dir_info_cache_list[file->dir_info_fd].info);
    ret = dir_info_cache_get(next_dir_info);
    if (ret < 0)
    {
        return ret;
    }

    file->dir_info_fd = ret;
    file->offset++;
    return sizeof(*_dir);
}

int appfs_open_init(fs_info_t *fs_info)
{
    for (int i = 0; i < DIR_INFO_NR; i++)
    {
        appfs_files[i].dir_info_fd = -1;
    }
    fs = fs_info;
    return 0;
}
