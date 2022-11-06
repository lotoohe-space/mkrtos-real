#include "syscalls.h"

#ifdef __NR_eventfd
syscall(eventfd,__eventfd)
#endif
