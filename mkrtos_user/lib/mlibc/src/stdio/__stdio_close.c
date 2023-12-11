#include "stdio_impl.h"
#include "aio_impl.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
static int dummy(int fd)
{
	return fd;
}

weak_alias(dummy, __aio_close);

int __stdio_close(FILE *f)
{
#ifdef NO_LITTLE_MODE
	return syscall(SYS_close, __aio_close(f->fd));
#else
	return be_close(__aio_close(f->fd));
#endif
}
