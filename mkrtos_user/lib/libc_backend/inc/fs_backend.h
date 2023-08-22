#pragma once

#include <stdarg.h>

long sys_be_ioctl(va_list ap);
long sys_be_writev(va_list ap);
long sys_be_write(va_list ap);
long sys_be_read(va_list ap);

