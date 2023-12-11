#include <unistd.h>
#include <fcntl.h>
#include "syscall.h"

#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif

int symlink(const char *existing, const char *new)
{
#ifdef SYS_symlink
#ifdef NO_LITTLE_MODE
	return syscall(SYS_symlink, existing, new);
#else
	return be_symlink(existing, new);
#endif
#else
	return syscall(SYS_symlinkat, existing, AT_FDCWD, new);
#endif
}
