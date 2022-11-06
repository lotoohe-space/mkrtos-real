/*
 * mkrtos_strlen.c
 *
 *  Created on: 2022Äê9ÔÂ4ÈÕ
 *      Author: Administrator
 */
#include <type.h>
static const unsigned long magic = 0x01010101;

size_t mkrtos_strlen(const char *s)
{
  const char *t = s;
  unsigned long word;

  if (!s) return 0;

  for (;*s++;);
  return (size_t)(s - t) - 1;
}
