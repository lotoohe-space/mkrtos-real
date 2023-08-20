#include <string.h>

char*strtok_r(char*s,const char*delim,char**ptrptr) {
  char*tmp=0;

  if (s==0) s=*ptrptr;
  s+=strspn(s,delim);		/* overread leading delimiter */
  if ((*s)) {
    tmp=s;
    s+=strcspn(s,delim);
    if ((*s)) *s++=0;	/* not the end ? => terminate it */
  }
  *ptrptr=s;
  return tmp;
}
