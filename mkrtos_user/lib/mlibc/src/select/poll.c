#include <poll.h>
#include <time.h>
#include <signal.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
int poll(struct pollfd *fds, nfds_t n, int timeout)
{
#ifdef SYS_poll
#ifdef NO_LITTLE_MODE
	return syscall_cp(SYS_poll, fds, n, timeout);
#else
	return be_poll(fds, n, timeout);
#endif
#else
#if SYS_ppoll_time64 == SYS_ppoll
	typedef long long ppoll_ts_t[2];
#else
	typedef long ppoll_ts_t[2];
#endif
	return syscall_cp(SYS_ppoll, fds, n, timeout>=0 ?
		((ppoll_ts_t){ timeout/1000, timeout%1000*1000000 }) : 0,
		0, _NSIG/8);
#endif
}
