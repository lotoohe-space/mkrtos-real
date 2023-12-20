#include <stdio.h>
#include <stdlib.h>
#include <cons_cli.h>
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
	cons_write_str("Assertion failed:");
	cons_write_str(expr);
	cons_write_str(file);
	cons_write_str(func);
	itoa(data, line);
	cons_write_str(data);
	cons_write_str("\n");
	abort();
}
