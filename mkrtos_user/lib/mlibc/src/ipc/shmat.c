#include <sys/shm.h>
#include "syscall.h"
#include "ipc.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
#ifdef NO_LITTLE_MODE
#ifndef SYS_ipc
void *shmat(int id, const void *addr, int flag)
{
	return (void *)syscall(SYS_shmat, id, addr, flag);
}
#else
void *shmat(int id, const void *addr, int flag)
{
	unsigned long ret;
	ret = syscall(SYS_ipc, IPCOP_shmat, id, flag, &addr, addr);
	return (ret > -(unsigned long)SHMLBA) ? (void *)ret : (void *)addr;
}
#endif
#else
void *shmat(int id, const void *addr, int flag)
{
	return (void *)__syscall_ret((long)be_shmat(id, addr, flag));
}
#endif
