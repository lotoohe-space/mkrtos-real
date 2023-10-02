#include <stdlib.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
_Noreturn void _Exit(int ec)
{
	__syscall(SYS_exit_group, ec);
#ifdef NO_LITTLE_MODE
	for (;;)
		__syscall(SYS_exit, ec);
#else
	for (;;)
		be_exit(ec);
#endif
}
