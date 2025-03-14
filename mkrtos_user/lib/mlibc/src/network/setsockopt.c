#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
#define IS32BIT(x) !((x)+0x80000000ULL>>32)
#define CLAMP(x) (int)(IS32BIT(x) ? (x) : 0x7fffffffU+((0ULL+(x))>>63))

int setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
	const struct timeval *tv;
	time_t s;
	suseconds_t us;
#ifdef NO_LITTLE_MODE
	int r = __socketcall(setsockopt, fd, level, optname, optval, optlen, 0);
#else
	int r = be_setsockopt(fd, level, optname, optval, optlen);
#endif
	if (r==-ENOPROTOOPT) switch (level) {
	case SOL_SOCKET:
		switch (optname) {
		case SO_RCVTIMEO:
		case SO_SNDTIMEO:
			if (SO_RCVTIMEO == SO_RCVTIMEO_OLD) break;
			if (optlen < sizeof *tv) return __syscall_ret(-EINVAL);
			tv = optval;
			s = tv->tv_sec;
			us = tv->tv_usec;
			if (!IS32BIT(s)) return __syscall_ret(-ENOTSUP);

			if (optname==SO_RCVTIMEO) optname=SO_RCVTIMEO_OLD;
			if (optname==SO_SNDTIMEO) optname=SO_SNDTIMEO_OLD;
#ifdef NO_LITTLE_MODE
			r = __socketcall(setsockopt, fd, level, optname,
				((long[]){s, CLAMP(us)}), 2*sizeof(long), 0);
#else
			r = be_setsockopt(fd, level, optname,
				((long[]){s, CLAMP(us)}), 2*sizeof(long));
#endif
			break;
		case SO_TIMESTAMP:
		case SO_TIMESTAMPNS:
			if (SO_TIMESTAMP == SO_TIMESTAMP_OLD) break;
			if (optname==SO_TIMESTAMP) optname=SO_TIMESTAMP_OLD;
			if (optname==SO_TIMESTAMPNS) optname=SO_TIMESTAMPNS_OLD;
#ifdef NO_LITTLE_MODE
			r = __socketcall(setsockopt, fd, level,
				optname, optval, optlen, 0);
#else
			r = be_setsockopt(fd, level, optname, optval, optlen);
#endif
			break;
		}
	}
	return __syscall_ret(r);
}
