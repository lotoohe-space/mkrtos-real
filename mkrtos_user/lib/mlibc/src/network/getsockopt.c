#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
int getsockopt(int fd, int level, int optname, void *restrict optval, socklen_t *restrict optlen)
{
	long tv32[2];
	struct timeval *tv;
#ifdef NO_LITTLE_MODE
	int r = __socketcall(getsockopt, fd, level, optname, optval, optlen, 0);
#else
	int r = be_getsockopt(fd, level, optname, optval, optlen);
#endif
	if (r==-ENOPROTOOPT) switch (level) {
	case SOL_SOCKET:
		switch (optname) {
		case SO_RCVTIMEO:
		case SO_SNDTIMEO:
			if (SO_RCVTIMEO == SO_RCVTIMEO_OLD) break;
			if (*optlen < sizeof *tv) return __syscall_ret(-EINVAL);
			if (optname==SO_RCVTIMEO) optname=SO_RCVTIMEO_OLD;
			if (optname==SO_SNDTIMEO) optname=SO_SNDTIMEO_OLD;
#ifdef NO_LITTLE_MODE
			r = __socketcall(getsockopt, fd, level, optname,
				tv32, (socklen_t[]){sizeof tv32}, 0);
#else
			r = be_getsockopt(fd, level, optname, tv32, (socklen_t[]){sizeof tv32});
#endif
			if (r<0) break;
			tv = optval;
			tv->tv_sec = tv32[0];
			tv->tv_usec = tv32[1];
			*optlen = sizeof *tv;
			break;
		case SO_TIMESTAMP:
		case SO_TIMESTAMPNS:
			if (SO_TIMESTAMP == SO_TIMESTAMP_OLD) break;
			if (optname==SO_TIMESTAMP) optname=SO_TIMESTAMP_OLD;
			if (optname==SO_TIMESTAMPNS) optname=SO_TIMESTAMPNS_OLD;
#ifdef NO_LITTLE_MODE
			r = __socketcall(getsockopt, fd, level,
				optname, optval, optlen, 0);
#else
			r = be_getsockopt(fd, level, optname, optval, optlen);
#endif
			break;
		}
	}
	return __syscall_ret(r);
}
