#include <sys/socket.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
int getpeername(int fd, struct sockaddr *restrict addr, socklen_t *restrict len)
{
#ifdef NO_LITTLE_MODE
	return socketcall(getpeername, fd, addr, len, 0, 0, 0);
#else
	return __syscall_ret(be_getpeername(fd, addr, len));
#endif
}
