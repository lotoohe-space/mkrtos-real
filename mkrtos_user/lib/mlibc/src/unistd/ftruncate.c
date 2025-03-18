#include <unistd.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
int ftruncate(int fd, off_t length)
{
#ifdef NO_LITTLE_MODE
	return syscall(SYS_ftruncate, fd, __SYSCALL_LL_O(length));
#else
	return __syscall_ret(be_ftruncate(fd, length));
#endif
}
