#include <mqueue.h>
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
int mq_close(mqd_t mqd)
{
#ifdef NO_LITTLE_MODE
	return syscall(SYS_close, mqd);
#else
	return be_close(mqd);
#endif
}
