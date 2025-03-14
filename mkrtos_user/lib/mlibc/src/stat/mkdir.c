#include <sys/stat.h>
#include <fcntl.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
int mkdir(const char *path, mode_t mode)
{
#ifdef SYS_mkdir
#ifdef NO_LITTLE_MODE
	return syscall(SYS_mkdir, path, mode);
#else
	return be_mkdir(path, mode);
#endif
#else
	return syscall(SYS_mkdirat, AT_FDCWD, path, mode);
#endif
}
