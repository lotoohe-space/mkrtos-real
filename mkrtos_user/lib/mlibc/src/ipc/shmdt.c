#include <sys/shm.h>
#include "syscall.h"
#include "ipc.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
int shmdt(const void *addr)
{
#ifdef NO_LITTLE_MODE
#ifndef SYS_ipc
	return syscall(SYS_shmdt, addr);
#else
	return syscall(SYS_ipc, IPCOP_shmdt, 0, 0, 0, addr);
#endif
#else
	return __syscall_ret(be_shmdt(addr));
#endif
}
