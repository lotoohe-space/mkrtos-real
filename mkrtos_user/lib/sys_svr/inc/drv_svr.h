#pragma once

#include <u_types.h>
#include <stddef.h>
#include <drv_types.h>
int dev_open(const char *name, uint32_t oflags);
int dev_close(int desc);
int dev_read(int desc, void *buf, size_t size);
int dev_write(int desc, const void *buf, size_t size);
int dev_ioctl(int desc, int cmd, void *args);

void drv_init(drv_t *drv);
