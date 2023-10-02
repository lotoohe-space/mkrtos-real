#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
int isatty(int fd)
{
	struct winsize wsz;
#ifdef NO_LITTLE_MODE
	unsigned long r = syscall(SYS_ioctl, fd, TIOCGWINSZ, &wsz);
#else
	unsigned long r = be_ioctl(fd, TIOCGWINSZ, &wsz);
#endif
	if (r == 0)
		return 1;
	if (errno != EBADF)
		errno = ENOTTY;
	return 0;
}
