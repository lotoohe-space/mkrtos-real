#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
/* for environ: */
#include <stdlib.h>

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

int __libc_getpagesize(void);
int __libc_getpagesize(void) {
  long* x=(long*)environ;
  int fd;
  while (*x) ++x;	/* skip envp to get to auxvec */
  ++x;

  while (*x) {
    if (*x==6)
      return x[1];
    x+=2;
  }
  return PAGE_SIZE;
}

int getpagesize(void)       __attribute__((weak,alias("__libc_getpagesize")));

