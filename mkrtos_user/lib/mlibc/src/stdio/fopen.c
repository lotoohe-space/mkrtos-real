#include "stdio_impl.h"
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
FILE *fopen(const char *restrict filename, const char *restrict mode)
{
	FILE *f;
	int fd;
	int flags;

	/* Check for valid initial mode character */
	if (!strchr("rwa", *mode))
	{
		errno = EINVAL;
		return 0;
	}

	/* Compute the flags to pass to open() */
	flags = __fmodeflags(mode);
#ifdef NO_LITTLE_MODE
	fd = sys_open(filename, flags, 0666);
#else
	fd = be_open(filename, flags, 0666);
#endif
	if (fd < 0)
		return 0;
	if (flags & O_CLOEXEC)
#ifdef NO_LITTLE_MODE
		__syscall(SYS_fcntl, fd, F_SETFD, FD_CLOEXEC);
#else
		/*TODO:*/
		;
#endif

	f = __fdopen(fd, mode);
	if (f)
		return f;

	be_close(fd);
	return 0;
}
