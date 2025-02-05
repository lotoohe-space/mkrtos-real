#include <sys/socket.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
int bind(int fd, const struct sockaddr *addr, socklen_t len)
{
#ifdef NO_LITTLE_MODE
	return socketcall(bind, fd, addr, len, 0, 0, 0);
#else
	return __syscall_ret(be_bind(fd, addr, len));
#endif
}
