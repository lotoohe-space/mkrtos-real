#include <stdio.h>
#include <stdlib.h>
#include "fs_backend.h"
static char *itoa(char *p, unsigned x) {
	p += 3*sizeof(int);
	*--p = 0;
	do {
		*--p = '0' + x % 10;
		x /= 10;
	} while (x);
	return p;
}
_Noreturn void __assert_fail(const char *expr, const char *file, int line, const char *func)
{
	char data[12];
	// fprintf(stderr, "Assertion failed: %s (%s: %s: %d)\n", expr, file, func, line);
	fs_cons_write_str("Assertion failed:");
	fs_cons_write_str(expr);
	fs_cons_write_str(file);
	fs_cons_write_str(func);
	itoa(data, line);
	fs_cons_write_str(data);
	fs_cons_write_str("\n");
	abort();
}
