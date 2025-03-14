#include <sys/socket.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
int accept(int fd, struct sockaddr *restrict addr, socklen_t *restrict len)
{
#ifdef NO_LITTLE_MODE
	return socketcall_cp(accept, fd, addr, len, 0, 0, 0);
#else
	return be_accept(fd, addr, len);
#endif
}
