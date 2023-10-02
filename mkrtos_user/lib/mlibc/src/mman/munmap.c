#include <sys/mman.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
static void dummy(void) {}
weak_alias(dummy, __vm_wait);

int __munmap(void *start, size_t len)
{
	__vm_wait();
#ifdef NO_LITTLE_MODE
	return syscall(SYS_munmap, start, len);
#else
	return be_munmap(start, len);
#endif
}

weak_alias(__munmap, munmap);
