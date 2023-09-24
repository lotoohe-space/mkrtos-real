#pragma once

#include <stddef.h>
int fs_open(const char *path, int flags, int mode);
int fs_read(int fd, void *buf, size_t len);
int fs_write(int fd, void *buf, size_t len);
int fs_close(int fd);
int fs_lseek(int fd, int offs, int whence);