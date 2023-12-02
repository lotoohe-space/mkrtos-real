#define _GNU_SOURCE
#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>
#include "__dirent.h"
#include "syscall.h"
#ifndef NO_LITTLE_MODE
#include "syscall_backend.h"
#endif
DIR *opendir(const char *name)
{
	int fd;
	DIR *dir;

#ifdef NO_LITTLE_MODE
	if ((fd = open(name, O_RDONLY | O_DIRECTORY | O_CLOEXEC)) < 0)
#else
	if ((fd = be_open(name, O_RDONLY | O_DIRECTORY | O_CLOEXEC, 0)) < 0)
#endif
		return 0;
	if (!(dir = calloc(1, sizeof *dir))) {
		be_close(fd);
		return 0;
	}
	dir->fd = fd;
	return dir;
}
