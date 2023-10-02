#include <stdlib.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include "syscall.h"

int posix_openpt(int flags)
{
	int r = open("/dev/ptmx", flags);
	if (r < 0 && errno == ENOSPC)
		errno = EAGAIN;
	return r;
}

int grantpt(int fd)
{
	return 0;
}

int unlockpt(int fd)
{
	int unlock = 0;
	return ioctl(fd, TIOCSPTLCK, &unlock);
}
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
int __ptsname_r(int fd, char *buf, size_t len)
{
	int pty, err;
	if (!buf)
		len = 0;
#ifdef NO_LITTLE_MODE
	if ((err = __syscall(SYS_ioctl, fd, TIOCGPTN, &pty)))
		return -err;
#else
	if ((err = be_ioctl(fd, TIOCGPTN, &pty)))
		return -err;
#endif
	if (snprintf(buf, len, "/dev/pts/%d", pty) >= len)
		return ERANGE;
	return 0;
}

weak_alias(__ptsname_r, ptsname_r);
