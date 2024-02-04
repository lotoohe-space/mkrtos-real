#include <elf.h>
#include <poll.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include "libc.h"

static size_t fake_auxv[20];
#define AUX_CNT 38

int __libc_start_main_tiny(int (*main)(int, char **, char **), int argc, char **argv,
                           void (*init_dummy)(), void (*fini_dummy)())
{
	char **envp = argv + argc + 1;
    
    size_t i, *auxv, aux[AUX_CNT] = {0};
	for (i = 0; envp[i]; i++)
		;
	libc.auxv = auxv = (void *)(envp + i + 1);
	for (i = 0; auxv[i]; i += 2)
	{
		if (auxv[i] < AUX_CNT)
		{
			aux[auxv[i]] = auxv[i + 1];
		}
		else if (auxv[i] == 0xfe)
		{
			extern void u_env_init(void **in_env);

			u_env_init((void *)auxv[i + 1]);
		}
	}

    libc.page_size = 512;
    libc.auxv = fake_auxv;
    __init_tls(fake_auxv);

    // _init();
    extern void *app_start_addr;
    extern weak hidden void (*const __init_array_start)(void), (*const __init_array_end)(void);

    unsigned long start_addr = ((unsigned long)app_start_addr) & (~3UL);

    uintptr_t a = (uintptr_t)&__init_array_start;
    for (; a < (uintptr_t)&__init_array_end; a += sizeof(void (*)()))
    {
        ((void (*)(void))((uintptr_t)(*((unsigned long *)a)) + start_addr | 0x1UL))();
    }
    exit(main(0, NULL, 0));
}