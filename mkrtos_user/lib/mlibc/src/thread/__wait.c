#include "pthread_impl.h"
#ifndef NO_LITTLE_MODE
#include <syscall_backend.h>
#endif
void __wait(volatile int *addr, volatile int *waiters, int val, int priv)
{
	int spins = 100;
	if (priv)
		priv = FUTEX_PRIVATE;
	while (spins-- && (!waiters || !*waiters))
	{
		if (*addr == val)
			a_spin();
		else
			return;
	}
	if (waiters)
		a_inc(waiters);
	while (*addr == val)
	{
#ifdef NO_LITTLE_MODE
		__syscall(SYS_futex, addr, FUTEX_WAIT | priv, val, 0) != -ENOSYS || __syscall(SYS_futex, addr, FUTEX_WAIT, val, 0);
#else
		be_futex((void *)addr, FUTEX_WAIT | priv, val, 0, 0, 0) != -ENOSYS || be_futex(addr, FUTEX_WAIT, val, 0, 0, 0);
#endif
	}
	if (waiters)
		a_dec(waiters);
}
