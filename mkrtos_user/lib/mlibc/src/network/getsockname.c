#include <sys/socket.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
int getsockname(int fd, struct sockaddr *restrict addr, socklen_t *restrict len)
{
#ifdef NO_LITTLE_MODE
	return socketcall(getsockname, fd, addr, len, 0, 0, 0);
#else
	return __syscall_ret(be_getsockname(fd, addr, len));
#endif
}
