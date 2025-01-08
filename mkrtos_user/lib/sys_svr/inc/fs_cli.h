#pragma once

#include <stddef.h>
#include <dirent.h>
#include <u_types.h>
#include <u_rpc.h>
sd_t fs_open(const char *path, int flags, int mode);
int fs_close(sd_t _fd);
int fs_read(sd_t _fd, void *buf, size_t len);
int fs_write(sd_t _fd, void *buf, size_t len);
int fs_readdir(sd_t _fd, dirent_t *dirent);
int fs_lseek(sd_t _fd, int offs, int whence);
int fs_ftruncate(sd_t _fd, off_t off);
int fs_fstat(sd_t _fd, stat_t *stat);
int fs_ioctl(sd_t _fd, int req, void *arg);
int fs_fcntl(sd_t _fd, int cmd, void *arg);
int fs_fsync(sd_t _fd);
int fs_unlink(const char *path);
int fs_symlink(const char *src, const char *dst);
int fs_mkdir(char *path);
int fs_rmdir(char *path);
int fs_rename(char *old, char *new);
int fs_stat(char *path, void *buf);
int fs_readlink(const char *path, char *buf, int bufsize);
int fs_statfs(const char *path, statfs_t *buf);
