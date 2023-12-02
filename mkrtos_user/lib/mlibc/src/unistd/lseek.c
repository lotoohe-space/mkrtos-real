#include <unistd.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
off_t __lseek(int fd, off_t offset, int whence)
{
#ifdef SYS__llseek
	off_t result;
	return syscall(SYS__llseek, fd, offset >> 32, offset, &result, whence) ? -1 : result;
#else
#ifdef NO_LITTLE_MODE
	return syscall(SYS_lseek, fd, offset, whence);
#else
	return be_lseek(fd, offset, whence);
#endif
#endif
}

weak_alias(__lseek, lseek);
