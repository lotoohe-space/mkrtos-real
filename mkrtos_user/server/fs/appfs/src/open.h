#pragma once
#include <sys/stat.h>
#include <dirent.h>
#include "appfs.h"
enum appfs_ioctl_cmd_op
{
    APPFS_IOCTOL_GET_ACCESS_ADDR,
};
typedef struct appfs_ioctl_arg
{
    unsigned long addr;
} appfs_ioctl_arg_t;

int appfs_open(const char *name, int flags, int mode);
int appfs_write(int fd, void *data, int len);
int appfs_read(int fd, void *data, int len);
int appfs_ioctl(int fd, unsigned long cmd, unsigned long arg);
int appfs_lseek(int fd, int offset, unsigned long whence);
int appfs_stat(int fd, struct stat *st);
int appfs_close(int fd);
int appfs_remove(const char *name);
int appfs_truncate(int fd, off_t length);

int appfs_readdir(int fd, struct dirent *_dir);

int appfs_open_init(fs_info_t *fs_info);
