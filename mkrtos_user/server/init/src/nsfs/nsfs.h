#pragma once
#include <fcntl.h>
#include <dirent.h>
#include "ns.h"
#include "kstat.h"
int fs_ns_open(const char *name, int flags, int mode);
int fs_ns_write(int fd, void *data, int len);
int fs_ns_read(int fd, void *data, int len);
int fs_ns_ioctl(int fd, unsigned long cmd, unsigned long arg);
int fs_ns_lseek(int fd, int offset, unsigned long whence);
int fs_ns_truncate(int fd, off_t length);
int fs_ns_fstat(int fd, struct kstat *st);
int fs_ns_stat(const char *path, struct kstat *st);
int fs_ns_close(int fd);
int fs_ns_remove(const char *name);
int fs_ns_readdir(int fd, struct dirent *_dir);
int fs_ns_mkdir(char *path);
int fs_ns_rmdir(const char *name);
int fs_ns_open_init(void);
