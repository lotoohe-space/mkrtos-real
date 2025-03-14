#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
int socket(int domain, int type, int protocol)
{
	#ifdef NO_LITTLE_MODE
	int s = __socketcall(socket, domain, type, protocol, 0, 0, 0);
	#else
	int s = be_socket(domain, type, protocol);
	#endif
	if ((s==-EINVAL || s==-EPROTONOSUPPORT)
	    && (type&(SOCK_CLOEXEC|SOCK_NONBLOCK))) {
	#ifdef NO_LITTLE_MODE
		s = __socketcall(socket, domain,
			type & ~(SOCK_CLOEXEC|SOCK_NONBLOCK),
			protocol, 0, 0, 0);
	#else
		s = be_socket(domain,
			type & ~(SOCK_CLOEXEC|SOCK_NONBLOCK),
			protocol);
	#endif
		if (s < 0) return __syscall_ret(s);
	#ifdef NO_LITTLE_MODE
		if (type & SOCK_CLOEXEC)
			__syscall(SYS_fcntl, s, F_SETFD, FD_CLOEXEC);
		if (type & SOCK_NONBLOCK)
			__syscall(SYS_fcntl, s, F_SETFL, O_NONBLOCK);
	#else
	if (type & SOCK_CLOEXEC)
			be_fcntl(s, F_SETFD, FD_CLOEXEC);
		if (type & SOCK_NONBLOCK)
			be_fcntl(s, F_SETFL, O_NONBLOCK);
	#endif
	}
	return __syscall_ret(s);
}
