#pragma once

#include <stdarg.h>

long sys_be_open(va_list ap);
long sys_be_open_at(va_list ap);
long sys_be_close(va_list ap);
long sys_be_ioctl(va_list ap);
long sys_be_writev(va_list ap);
long sys_be_write(va_list ap);
long sys_be_read(va_list ap);
long sys_be_ftruncate(va_list ap);
long sys_be_getdents(va_list ap);
long sys_be_lseek(va_list ap);
long sys_be_ioctl(va_list ap);
