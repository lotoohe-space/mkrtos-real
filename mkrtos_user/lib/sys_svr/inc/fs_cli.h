#pragma once

#include <stddef.h>
#include <dirent.h>
#include <u_types.h>
int fs_open(const char *path, int flags, int mode);
int fs_read(sd_t fd, void *buf, size_t len);
int fs_write(sd_t fd, void *buf, size_t len);
int fs_close(sd_t fd);
int fs_lseek(sd_t fd, int offs, int whence);
int fs_ftruncate(sd_t _fd, off_t off);
int fs_fsync(sd_t _fd);
int fs_readdir(sd_t _fd, struct dirent *dirent);
int fs_symlink(const char *src, const char *dst);
