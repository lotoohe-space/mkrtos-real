#include <sys/socket.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
int shutdown(int fd, int how)
{
#ifdef NO_LITTLE_MODE
	return socketcall(shutdown, fd, how, 0, 0, 0, 0);
#else
	return __syscall_ret(be_shutdown(fd, how));
#endif
}
