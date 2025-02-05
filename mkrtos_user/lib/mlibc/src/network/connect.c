#include <sys/socket.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
int connect(int fd, const struct sockaddr *addr, socklen_t len)
{
#ifdef NO_LITTLE_MODE
	return socketcall_cp(connect, fd, addr, len, 0, 0, 0);
#else
	return __syscall_ret(be_connect(fd, addr, len));
#endif
}
