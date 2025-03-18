#include <sys/shm.h>
#include <stdint.h>
#include "syscall.h"
#include "ipc.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
int shmget(key_t key, size_t size, int flag)
{
	if (size > PTRDIFF_MAX) size = SIZE_MAX;
#ifdef NO_LITTLE_MODE
#ifndef SYS_ipc
	return syscall(SYS_shmget, key, size, flag);
#else
	return syscall(SYS_ipc, IPCOP_shmget, key, size, flag);
#endif
#else
	return __syscall_ret(be_shmget(key, size, flag));
#endif
}
