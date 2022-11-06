/*
 * mkrtos_memccpy.c
 *
 *  Created on: 2022Äê8ÔÂ6ÈÕ
 *      Author: Administrator
 */
#include <type.h>
void *mkrtos_memccpy(void *dst, const void *src, int c, uint32_t count)
{
  char *a = dst;
  const char *b = src;
  while (count--)
  {
    *a++ = *b;
    if (*b==c)
    {
      return (void *)a;
    }
    b++;
  }
  return 0;
}
