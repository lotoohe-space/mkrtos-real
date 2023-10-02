#include <termios.h>
#include <sys/ioctl.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
int tcgetwinsize(int fd, struct winsize *wsz)
{
#ifdef NO_LITTLE_MODE
	return syscall(SYS_ioctl, fd, TIOCGWINSZ, wsz);
#else
	return be_ioctl(fd, TIOCGWINSZ, wsz);
#endif
}
