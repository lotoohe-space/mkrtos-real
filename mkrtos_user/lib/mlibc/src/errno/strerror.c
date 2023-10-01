#include <errno.h>
#include <stddef.h>
#include <string.h>
#include "locale_impl.h"

/* mips has one error code outside of the 8-bit range due to a
 * historical typo, so we just remap it. */
#if EDQUOT == 1133
#define EDQUOT_ORIG 1133
#undef EDQUOT
#define EDQUOT 109
#endif

static const struct errmsgstr_t
{
#ifdef NO_LITTLE_MODE
#define E(n, s) char str##n[sizeof(s)];
#include "__strerror.h"
#undef E
} errmsgstr = {
#define E(n, s) s,
#include "__strerror.h"
#undef E
#endif
};

static const unsigned short errmsgidx[] = {
#ifdef NO_LITTLE_MODE
#define E(n, s) [n] = offsetof(struct errmsgstr_t, str##n),
#include "__strerror.h"
#undef E
#endif
};

char *__strerror_l(int e, locale_t loc)
{
#ifdef NO_LITTLE_MODE
	const char *s;
#ifdef EDQUOT_ORIG
	if (e == EDQUOT)
		e = 0;
	else if (e == EDQUOT_ORIG)
		e = EDQUOT;
#endif
	if (e >= sizeof errmsgidx / sizeof *errmsgidx)
		e = 0;
	s = (char *)&errmsgstr + errmsgidx[e];
	return (char *)LCTRANS(s, LC_MESSAGES, loc);
#else
	return "error.";
#endif
}

char *strerror(int e)
{
	return __strerror_l(e, CURRENT_LOCALE);
}

weak_alias(__strerror_l, strerror_l);
