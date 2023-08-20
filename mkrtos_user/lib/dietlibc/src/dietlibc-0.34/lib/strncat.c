#include "dietfeatures.h"
#include <string.h>

/* gcc is broken and has a non-SUSv2 compliant internal prototype.
 * This causes it to warn about a type mismatch here.  Ignore it. */
char *strncat(char *s, const char *t, size_t n) {
  char *dest=s;
  register char *max;
  s+=strlen(s);
  if (((max=s+n)==s)) goto fini;
  for (;;) {
    if ((!(*s = *t))) break;
                                       if ((++s==max)) break;
                                                                        ++t;
#ifndef WANT_SMALL_STRING_ROUTINES
    if ((!(*s = *t))) break;
                                       if ((++s==max)) break;
                                                                        ++t;
    if ((!(*s = *t))) break;
                                       if ((++s==max)) break;
                                                                        ++t;
    if ((!(*s = *t))) break;
                                       if ((++s==max)) break;
                                                                        ++t;
#endif
  }
  *s=0;
fini:
  return dest;
}
