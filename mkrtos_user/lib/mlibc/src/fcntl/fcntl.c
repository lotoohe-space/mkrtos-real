#define _GNU_SOURCE
#include <fcntl.h>
#include <stdarg.h>
#include <errno.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
int fcntl(int fd, int cmd, ...)
{
	unsigned long arg;
	va_list ap;
	va_start(ap, cmd);
	arg = va_arg(ap, unsigned long);
	va_end(ap);
	if (cmd == F_SETFL) arg |= O_LARGEFILE;
	#ifdef NO_LITTLE_MODE
	if (cmd == F_SETLKW) return syscall_cp(SYS_fcntl, fd, cmd, (void *)arg);
	#else
	if (cmd == F_SETLKW) return be_fcntl(fd, cmd, (void *)arg);
	#endif
	if (cmd == F_GETOWN) {
		struct f_owner_ex ex;
	#ifdef NO_LITTLE_MODE
		int ret = __syscall(SYS_fcntl, fd, F_GETOWN_EX, &ex);
	#else
		int ret = be_fcntl(fd, F_GETOWN_EX, &ex);
	#endif
	#ifdef NO_LITTLE_MODE
		if (ret == -EINVAL) return __syscall(SYS_fcntl, fd, cmd, (void *)arg);
	#else
		if (ret == -EINVAL) return be_fcntl(fd, cmd, (void *)arg);
	#endif
		if (ret) return __syscall_ret(ret);
		return ex.type == F_OWNER_PGRP ? -ex.pid : ex.pid;
	}
	if (cmd == F_DUPFD_CLOEXEC) {
	#ifdef NO_LITTLE_MODE
		int ret = __syscall(SYS_fcntl, fd, F_DUPFD_CLOEXEC, arg);
	#else
		int ret = be_fcntl( fd, F_DUPFD_CLOEXEC, arg);
	#endif
		if (ret != -EINVAL) {
			if (ret >= 0)
	#ifdef NO_LITTLE_MODE
				__syscall(SYS_fcntl, ret, F_SETFD, FD_CLOEXEC);
	#else
				be_fcntl(ret, F_SETFD, FD_CLOEXEC);
	#endif
			return __syscall_ret(ret);
		}
	#ifdef NO_LITTLE_MODE
		ret = __syscall(SYS_fcntl, fd, F_DUPFD_CLOEXEC, 0);
	#else
		ret = be_fcntl(fd, F_DUPFD_CLOEXEC, 0);
	#endif
		if (ret != -EINVAL) {
	#ifdef NO_LITTLE_MODE
			if (ret >= 0) __syscall(SYS_close, ret);
	#else
			if (ret >= 0) be_close(ret);
	#endif
			return __syscall_ret(-EINVAL);
		}
	#ifdef NO_LITTLE_MODE
		ret = __syscall(SYS_fcntl, fd, F_DUPFD, arg);
		if (ret >= 0) __syscall(SYS_fcntl, ret, F_SETFD, FD_CLOEXEC);
	#else
		ret = be_fcntl(fd, F_DUPFD, arg);
		if (ret >= 0) be_fcntl(ret, F_SETFD, FD_CLOEXEC);
	#endif
		return __syscall_ret(ret);
	}
	switch (cmd) {
	case F_SETLK:
	case F_GETLK:
	case F_GETOWN_EX:
	case F_SETOWN_EX:
	#ifdef NO_LITTLE_MODE
		return syscall(SYS_fcntl, fd, cmd, (void *)arg);
	#else
		return be_fcntl(fd, cmd, (void *)arg);
	#endif
	default:
	#ifdef NO_LITTLE_MODE
		return syscall(SYS_fcntl, fd, cmd, arg);
	#else
		return be_fcntl(fd, cmd, (void *)arg);
	#endif
	}
}
