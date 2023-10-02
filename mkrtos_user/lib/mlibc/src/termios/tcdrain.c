#include <termios.h>
#include <sys/ioctl.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
int tcdrain(int fd)
{
#ifdef NO_LITTLE_MODE
	return syscall_cp(SYS_ioctl, fd, TCSBRK, 1);
#else
	return be_ioctl(fd, TCSBRK, 1);
#endif
}
