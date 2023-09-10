#include <features.h>
#include "libc.h"

#define START "_start"

// #include "crt_arch.h"

int main();
weak void _init();
weak void _fini();
int __libc_start_main(int (*)(), int, char **,
					  void (*)(), void (*)(), void (*)());
void *app_start_addr = NULL;
void _start_c(long *p, void *start_addr)
{
	int argc = p[0];
	char **argv = (void *)(p + 1);
	app_start_addr = start_addr; // zhangzheng add.

	__libc_start_main(main, argc, argv, _init, _fini, 0);
}
