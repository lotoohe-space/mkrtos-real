#include <unistd.h>
#include <fcntl.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
int access(const char *filename, int amode)
{
#ifdef SYS_access
#ifdef NO_LITTLE_MODE
	return syscall(SYS_access, filename, amode);
#else
	return be_access(filename, amode);
#endif
#else
	return syscall(SYS_faccessat, AT_FDCWD, filename, amode, 0);
#endif
}
