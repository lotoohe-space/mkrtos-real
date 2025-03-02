#pragma once

#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "u_types.h"

typedef struct fs_operations {
    int (*fs_svr_open)(const char *path, int flags, int mode);
    int (*fs_svr_read)(int fd, void *buf, size_t len);
    int (*fs_svr_write)(int fd, void *buf, size_t len);
    void (*fs_svr_close)(int fd);
    int (*fs_svr_readdir)(int fd, dirent_t *dir);
    int (*fs_svr_lseek)(int fd, int offs, int whence);
    int (*fs_svr_ftruncate)(int fd, off_t off);
    int (*fs_svr_fstat)(int fd, void *buf);
    int (*fs_svr_ioctl)(int fd, int req, void *arg);
    int (*fs_svr_fcntl)(int fd, int cmd, void *arg);
    int (*fs_svr_fsync)(int fd);
    int (*fs_svr_unlink)(const char *path);
    int (*fs_svr_symlink)(const char *existing, const char *new);
    int (*fs_svr_mkdir)(char *path);
    int (*fs_svr_rmdir)(char *path);
    int (*fs_svr_rename)(char *old, char *new);
    int (*fs_svr_stat)(const char *path, void *buf);
    ssize_t (*fs_svr_readlink)(const char *path, char *buf, size_t bufsize);
    int (*fs_svr_statfs)(const char *path, struct statfs *buf);
    //select
    //poll
} fs_operations_t;

typedef struct fs {
    rpc_svr_obj_t svr;
    const fs_operations_t *op;
} fs_t;

void fs_init(fs_t *fs, const fs_operations_t *op);

// int fs_svr_open(const char *path, int flags, int mode);
// int fs_svr_read(int fd, void *buf, size_t len);
// int fs_svr_write(int fd, void *buf, size_t len);
// void fs_svr_close(int fd);
// int fs_svr_readdir(int fd, dirent_t *dir);
// int fs_svr_lseek(int fd, int offs, int whence);
// int fs_svr_ftruncate(int fd, off_t off);
// int fs_svr_fstat(int fd, void *buf);
// int fs_svr_ioctl(int fd, int req, void *arg);
// int fs_svr_fcntl(int fd, int cmd, void *arg);
// int fs_svr_fsync(int fd);
// int fs_svr_unlink(const char *path);
// int fs_svr_symlink(const char *existing, const char *new);
// int fs_svr_mkdir(char *path);
// int fs_svr_rmdir(char *path);
// int fs_svr_rename(char *old, char *new);
// int fs_svr_stat(const char *path, void *buf);
// ssize_t fs_svr_readlink(const char *path, char *buf, size_t bufsize);
// int fs_svr_statfs(const char *path, struct statfs *buf);
