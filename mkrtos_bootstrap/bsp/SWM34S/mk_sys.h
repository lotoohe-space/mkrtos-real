#ifndef __SYS_H
#define __SYS_H	


#include <SWM341.h>

void jump2kernel(addr_t cpio_start, addr_t cpio_end);

#endif