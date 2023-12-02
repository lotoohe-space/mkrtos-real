#include <sys/uio.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
ssize_t readv(int fd, const struct iovec *iov, int count)
{
#ifdef NO_LITTLE_MODE
	return syscall_cp(SYS_readv, fd, iov, count);
#else
	return be_readv(fd, iov, count);
#endif
}
