#include <features.h>
#include "libc.h"

#define START "_start"

// #include "crt_arch.h"

int main();
weak void _init();
weak void _fini();
int __libc_start_main_init(int (*main)(int,char **,char **), int argc, char **argv,
	void (*init_dummy)(), void(*fini_dummy)(), void(*ldso_dummy)());

void _start_c_init(long *p)
{
	int argc = p[0];
	char **argv = (void *)(p+1);
	__libc_start_main_init(main, argc, argv, _init, _fini, 0);
}
