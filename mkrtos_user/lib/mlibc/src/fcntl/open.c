#include <fcntl.h>
#include <stdarg.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
int open(const char *filename, int flags, ...)
{
	mode_t mode = 0;

	if ((flags & O_CREAT) || (flags & O_TMPFILE) == O_TMPFILE)
	{
		va_list ap;
		va_start(ap, flags);
		mode = va_arg(ap, mode_t);
		va_end(ap);
	}

#ifdef NO_LITTLE_MODE
	int fd = __sys_open_cp(filename, flags, mode);
#else
	int fd = be_open(filename, flags, mode);
#endif
	if (fd >= 0 && (flags & O_CLOEXEC))
		__syscall(SYS_fcntl, fd, F_SETFD, FD_CLOEXEC);

	return __syscall_ret(fd);
}
