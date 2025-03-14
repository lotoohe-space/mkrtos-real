#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
char *getcwd(char *buf, size_t size)
{
	char tmp[buf ? 1 : PATH_MAX];
	if (!buf) {
		buf = tmp;
		size = sizeof tmp;
	} else if (!size) {
		errno = EINVAL;
		return 0;
	}

#ifdef NO_LITTLE_MODE
	long ret = syscall(SYS_getcwd, buf, size);
#else
	long ret = be_getcwd(buf, size);
#endif
	if (ret < 0)
		return 0;
	if (ret == 0 || buf[0] != '/') {
		errno = ENOENT;
		return 0;
	}
	return buf == tmp ? strdup(buf) : buf;
}
