#include "stdio_impl.h"
#include <sys/ioctl.h>
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
size_t __stdout_write(FILE *f, const unsigned char *buf, size_t len)
{
	struct winsize wsz;
	f->write = __stdio_write;
#ifdef NO_LITTLE_MODE
	if (!(f->flags & F_SVB) && __syscall(SYS_ioctl, f->fd, TIOCGWINSZ, &wsz))
		f->lbf = -1;
#else
	if (!(f->flags & F_SVB) && be_ioctl(f->fd, TIOCGWINSZ, &wsz))
		f->lbf = -1;
#endif
	return __stdio_write(f, buf, len);
}
