#pragma once

#include <stddef.h>
#include <u_types.h>

int dev_fs_open(const char *path, uint32_t oflags);
int dev_fs_close(int desc);
int dev_fs_read(int desc, void *buf, size_t size);
int dev_fs_write(int desc, const void *buf, size_t size);
int dev_fs_ioctl(int desc, int cmd, void *args);
