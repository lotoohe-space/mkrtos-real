
#include "types.h"
#include "string.h"

int memcmp(const void *vl, const void *vr, size_t n)
{
	const unsigned char *l=vl, *r=vr;
	for (; n && *l == *r; n--, l++, r++);
	return n ? *l-*r : 0;
}
void *memccpy(void *dst, const void *src, int c, size_t count)
{
    char *a = dst;
    const char *b = src;
    while (count--)
    {
        *a++ = *b;
        if (*b == c)
        {
            return (void *)a;
        }
        b++;
    }
    return 0;
}
void memcpy(void *dst, void *src, size_t len)
{
    register char *b_dst = dst;
    register char *b_src = src;
    for (; len; len--)
    {
        (*b_dst++) = (*b_src++);
    }
}
void *memset(void *dst, char s, size_t count)
{
    register char *a = dst;
    count++; /* this actually creates smaller code than using count-- */
    while (--count)
        *a++ = s;
    return dst;
}
int strcmp(const char *s1, const char *s2)
{
    while (*s1 && *s1 == *s2)
        s1++, s2++;
    return (*s1 - *s2);
}
char *strcpy(register char *s1, register const char *s2)
{
    register char *res = s1;
    while ((*s1++ = *s2++))
        ;
    return (res);
}
size_t strlen(const char *s)
{
    const char *t = s;
    unsigned long word;

    if (!s)
        return 0;

    for (; *s++;)
        ;
    return (size_t)(s - t) - 1;
}
char *strncpy(char *dest, const char *src, uint32_t n)
{
    memccpy(dest, src, 0, n);
    return dest;
}
