#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "syscall.h"
#include "pthread_impl.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
struct ctx
{
	int fd;
	const char *filename;
	int amode;
	int p;
};

static int checker(void *p)
{
	struct ctx *c = p;
	int ret;
	if (__syscall(SYS_setregid, __syscall(SYS_getegid), -1) || __syscall(SYS_setreuid, __syscall(SYS_geteuid), -1))
#ifdef NO_LITTLE_MODE
		__syscall(SYS_exit, 1);
#else
		be_exit(1);
#endif

	ret = __syscall(SYS_faccessat, c->fd, c->filename, c->amode, 0);
#ifdef NO_LITTLE_MODE
	__syscall(SYS_write, c->p, &ret, sizeof ret);
#else
	be_write(c->p, (char*)&ret, sizeof ret);
#endif
	return 0;
}
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
int faccessat(int fd, const char *filename, int amode, int flag)
{
	if (flag)
	{
		int ret = __syscall(SYS_faccessat2, fd, filename, amode, flag);
		if (ret != -ENOSYS)
			return __syscall_ret(ret);
	}

	if (flag & ~AT_EACCESS)
		return __syscall_ret(-EINVAL);

	if (!flag || (getuid() == geteuid() && getgid() == getegid()))
		return syscall(SYS_faccessat, fd, filename, amode);

	char stack[1024];
	sigset_t set;
	pid_t pid;
	int status;
	int ret, p[2];

	if (pipe2(p, O_CLOEXEC))
		return __syscall_ret(-EBUSY);
	struct ctx c = {.fd = fd, .filename = filename, .amode = amode, .p = p[1]};

	__block_all_sigs(&set);

	pid = __clone(checker, stack + sizeof stack, 0, &c);
	be_close(p[1]);

#ifdef NO_LITTLE_MODE
	if (pid < 0 || __syscall(SYS_read, p[0], &ret, sizeof ret) != sizeof(ret))
		ret = -EBUSY;
#else
	if (pid < 0 || be_read(p[0], (char*)&ret, sizeof ret) != sizeof(ret))
		ret = -EBUSY;
#endif
	be_close(p[0]);
	__syscall(SYS_wait4, pid, &status, __WCLONE, 0);

	__restore_sigs(&set);

	return __syscall_ret(ret);
}
