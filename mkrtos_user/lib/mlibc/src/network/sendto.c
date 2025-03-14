#include <sys/socket.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
ssize_t sendto(int fd, const void *buf, size_t len, int flags, const struct sockaddr *addr, socklen_t alen)
{
#ifdef NO_LITTLE_MODE
	return socketcall_cp(sendto, fd, buf, len, flags, addr, alen);
#else
	return __syscall_ret(be_sendto(fd, buf, len, flags, addr, alen));
#endif
}
