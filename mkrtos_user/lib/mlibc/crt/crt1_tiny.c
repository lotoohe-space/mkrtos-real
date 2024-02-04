#include <features.h>
#include "libc.h"

#define START "_start"

// #include "crt_arch.h"

int main();
weak void _init();
weak void _fini();
int __libc_start_main_tiny(int (*main)(int, char **, char **), int argc, char **argv,
						   void (*init_dummy)(), void (*fini_dummy)());
extern void *app_start_addr;
void _start_tiny_init(long *p, void *start_addr)
{
	int argc = p[0];
	char **argv = (void *)(p + 1);

	app_start_addr = start_addr; // ATShining add.
	__libc_start_main_tiny(main, argc, argv, _init, _fini);
}
