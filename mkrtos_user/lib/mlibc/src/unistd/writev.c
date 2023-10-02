#include <sys/uio.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
ssize_t writev(int fd, const struct iovec *iov, int count)
{
#ifdef NO_LITTLE_MODE
	return syscall_cp(SYS_writev, fd, iov, count);
#else
	return be_writev(fd, iov, count);
#endif
}
