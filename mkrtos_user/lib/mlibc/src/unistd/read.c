#include <unistd.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
ssize_t read(int fd, void *buf, size_t count)
{
#ifdef NO_LITTLE_MODE
	return syscall_cp(SYS_read, fd, buf, count);
#else
	return __syscall_ret(be_read(fd, buf, count));
#endif
}
