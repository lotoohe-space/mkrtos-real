#pragma once
#include <sys/stat.h>
#include <dirent.h>
#include "appfs.h"
#ifdef MKRTOS
#include "kstat.h"
#include <u_types.h>
#else
#include <sys/stat.h>
struct kstat {
	long st_dev;
	int __st_dev_padding;
	long __st_ino_truncated;
	mode_t st_mode;
	nlink_t st_nlink;
	uid_t st_uid;
	gid_t st_gid;
	long st_rdev;
	int __st_rdev_padding;
	long st_size;
	int st_blksize;
	long st_blocks;
	long st_atime_sec;
	long st_atime_nsec;
	long st_mtime_sec;
	long st_mtime_nsec;
	long st_ctime_sec;
	long st_ctime_nsec;
	long st_ino;
};

#endif
enum appfs_ioctl_cmd_op
{
    APPFS_IOCTOL_GET_ACCESS_ADDR,
};
typedef struct appfs_ioctl_arg
{
    unsigned long addr;
    unsigned long size;
} appfs_ioctl_arg_t;

void appfs_task_free(int pid);
int appfs_open(const char *name, int flags, int mode);
int appfs_write(int fd, void *data, int len);
int appfs_read(int fd, void *data, int len);
int appfs_ioctl(int fd, unsigned long cmd, unsigned long arg);
int appfs_lseek(int fd, int offset, unsigned long whence);
int appfs_fstat(int fd, struct kstat *st);
int appfs_stat(const char *path, struct kstat *st);
int appfs_close(int fd);
int appfs_remove(const char *name);
int appfs_truncate(int fd, off_t length);

int appfs_readdir(int fd, struct dirent *_dir);

int appfs_open_init(fs_info_t *fs_info);
