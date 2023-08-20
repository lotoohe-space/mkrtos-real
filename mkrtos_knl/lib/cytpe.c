
#include "ctype.h"

int isupper(int ch)
{
  return (unsigned int)(ch - 'A') < 26u;
}
int islower(int ch)
{
  return (unsigned int)(ch - 'a') < 26u;
}
int tolower(int ch)
{
  if ((unsigned int)(ch - 'A') < 26u)
    ch += 'a' - 'A';
  return ch;
}
int toupper(int ch)
{
  if ((unsigned int)(ch - 'a') < 26u)
    ch += 'A' - 'a';
  return ch;
}
int iscntrl(int ch)
{
  return (unsigned int)ch < 32u || ch == 127;
}
