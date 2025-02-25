#include <unistd.h>
#include "syscall.h"

#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
int chdir(const char *path)
{
#ifdef NO_LITTLE_MODE
	return syscall(SYS_chdir, path);
#else
	return be_chdir(path);
#endif
}
