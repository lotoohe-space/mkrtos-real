#pragma once

#include "u_rpc_svr.h"
#include "u_types.h"
#include "u_rpc.h"
typedef struct fs
{
    rpc_svr_obj_t svr;
} fs_t;

void fs_init(fs_t *fs);

int fs_svr_open(const char *path, int flags, int mode);
int fs_svr_read(int fd, void *buf, size_t len);
int fs_svr_write(int fd, void *buf, size_t len);
void fs_svr_close(int fd);
int fs_svr_lseek(int fd, int offs, int whence);
int fs_svr_ftruncate(int fd, off_t off);
void fs_svr_sync(int fd);
int fs_svr_readdir(int fd, dirent_t *dir);
int fs_svr_mkdir(char *path);
int fs_svr_unlink(char *path);
int fs_svr_renmae(char *oldname, char *newname);
int fs_svr_fstat(int fd, stat_t *stat);
int fs_svr_symlink(const char *src, const char *dst);
