#include "pthread_impl.h"
#include "atomic.h"
#include "syscall.h"
/* cheat and reuse CRTJMP macro from dynlink code */
#include "dynlink.h"
static void *unmap_base;
static size_t unmap_size;
static char shared_stack[256];
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
static void do_unmap()
{
#ifdef NO_LITTLE_MODE
	__syscall(SYS_munmap, unmap_base, unmap_size);
	__syscall(SYS_exit);
#else
	be_munmap(unmap_base, unmap_size);
	be_exit(0);
#endif
}

void __unmapself(void *base, size_t size)
{
	char *stack = shared_stack + sizeof shared_stack;
	stack -= (uintptr_t)stack % 16;
	unmap_base = base;
	unmap_size = size;
	CRTJMP(do_unmap, stack);
}
