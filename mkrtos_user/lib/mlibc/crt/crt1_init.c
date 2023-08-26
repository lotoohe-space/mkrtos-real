#include <features.h>
#include "libc.h"

#define START "_start"

// #include "crt_arch.h"

int main();
weak void _init();
weak void _fini();
int __libc_start_main_init(int (*main)(int,char **,char **), int argc, char **argv,
	void (*init_dummy)(), void(*fini_dummy)(), void(*ldso_dummy)());

void _start_c_init(void)
{
	__libc_start_main_init(main, NULL, NULL, _init, _fini, 0);
}
