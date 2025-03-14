#include <sys/socket.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
int listen(int fd, int backlog)
{
#ifdef NO_LITTLE_MODE
	return socketcall(listen, fd, backlog, 0, 0, 0, 0);
#else
	return __syscall_ret(be_listen(fd, backlog));
#endif
}
