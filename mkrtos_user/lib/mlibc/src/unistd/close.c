#include <unistd.h>
#include <errno.h>
#include "aio_impl.h"
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
static int dummy(int fd)
{
	return fd;
}

weak_alias(dummy, __aio_close);

int close(int fd)
{
	fd = __aio_close(fd);
#ifdef NO_LITTLE_MODE
	int r = __syscall_cp(SYS_close, fd);
#else
	int r = be_close(fd);
#endif
	if (r == -EINTR)
		r = 0;
	return __syscall_ret(r);
}
