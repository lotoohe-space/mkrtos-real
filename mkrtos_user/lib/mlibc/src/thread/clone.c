#include <errno.h>
#include "pthread_impl.h"
#include <syscall_backend.h>
int __clone(int (*func)(void *), void *stack, int flags, void *arg, ...)
{
	pid_t *ptid;
	void *tls;
	pid_t *ctid;

	va_list ap;

	va_start(ap, arg);
	ARG_3_BE(ap, ptid, void*, tls, void*, ctid, void*);
	va_end(ap);

	return be_clone(func, stack, flags, arg, ptid, tls, ctid);
}
