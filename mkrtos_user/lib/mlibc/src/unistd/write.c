#include <unistd.h>
#include "syscall.h"

#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
ssize_t write(int fd, const void *buf, size_t count)
{
#ifdef NO_LITTLE_MODE
	return syscall_cp(SYS_write, fd, buf, count);
#else
	return be_write(fd, buf, count);
#endif
}
