#include <termios.h>
#include <sys/ioctl.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
int tcsetwinsize(int fd, const struct winsize *wsz)
{
#ifdef NO_LITTLE_MODE
	return syscall(SYS_ioctl, fd, TIOCSWINSZ, wsz);
#else
	return be_ioctl(fd, TIOCSWINSZ, wsz);

#endif
}
