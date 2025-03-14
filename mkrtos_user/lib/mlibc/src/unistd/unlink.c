#include <unistd.h>
#include <fcntl.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
int unlink(const char *path)
{
#ifdef SYS_unlink
#ifdef NO_LITTLE_MODE
	return syscall(SYS_unlink, path);
#else
	return be_unlink(path);
#endif
#else
	return syscall(SYS_unlinkat, AT_FDCWD, path, 0);
#endif
}
