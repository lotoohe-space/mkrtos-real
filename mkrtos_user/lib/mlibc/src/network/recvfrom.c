#include <sys/socket.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
ssize_t recvfrom(int fd, void *restrict buf, size_t len, int flags, struct sockaddr *restrict addr, socklen_t *restrict alen)
{
#ifdef NO_LITTLE_MODE
	return socketcall_cp(recvfrom, fd, buf, len, flags, addr, alen);
#else
	return __syscall_ret(be_recvfrom(fd, buf, len, flags, addr, alen));
#endif
}
