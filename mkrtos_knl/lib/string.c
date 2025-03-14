
#include "types.h"
#include "string.h"

int memcmp(const void *vl, const void *vr, size_t n)
{
    const unsigned char *l = vl, *r = vr;
    for (; n && *l == *r; n--, l++, r++)
        ;
    return n ? *l - *r : 0;
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
void *memcpy(void *dst, const void *src, size_t len)
{
    register char *b_dst = dst;
    register char *b_src = (char *)src;

    for (; len; len--)
    {
        (*b_dst++) = (*b_src++);
    }
    return dst;
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

char *strchrnul(const char *s, int c)
{
    c = (unsigned char)c;
    if (!c)
        return (char *)s + strlen(s);

    for (; *s && *(unsigned char *)s != c; s++)
        ;
    return (char *)s;
}

char *strchr(const char *s, int c)
{
    char *r = strchrnul(s, c);
    return *(unsigned char *)r == (unsigned char)c ? r : 0;
}

void *memmove(void *dest, const void *src, size_t n)
{
    char *d = dest;
    const char *s = src;

    if (d == s)
        return d;
    if ((uintptr_t)s - (uintptr_t)d - n <= -2 * n)
        return memcpy(d, s, n);

    if (d < s)
    {

        for (; n; n--)
            *d++ = *s++;
    }
    else
    {

        while (n)
            n--, d[n] = s[n];
    }

    return dest;
}
void *memchr(const void *src, int c, size_t n)
{
    const unsigned char *s = src;
    c = (unsigned char)c;
    for (; n && *s != c; s++, n--)
        ;
    return n ? (void *)s : 0;
}

void *memrchr(const void *m, int c, size_t n)
{
    const unsigned char *s = m;
    c = (unsigned char)c;
    while (n--)
        if (s[n] == c)
            return (void *)(s + n);
    return 0;
}
char *strrchr(const char *s, int c)
{
    return memrchr(s, c, strlen(s) + 1);
}
size_t strnlen(const char *s, size_t n)
{
    const char *p = memchr(s, 0, n);
    return p ? p - s : n;
}
#define TOLOWER(x) ((x) | 0x20)
#define isxdigit(c) (('0' <= (c) && (c) <= '9') || ('a' <= (c) && (c) <= 'f') || ('A' <= (c) && (c) <= 'F'))

#define isdigit(c) ('0' <= (c) && (c) <= '9')

unsigned long strtoul(const char *cp, char **endp, unsigned int base)
{
    unsigned long result = 0, value;

    if (!base)
    {
        base = 10;
        if (*cp == '0')
        {
            base = 8;
            cp++;
            if ((TOLOWER(*cp) == 'x') && isxdigit(cp[1]))
            {
                cp++;
                base = 16;
            }
        }
    }
    else if (base == 16)
    {
        if (cp[0] == '0' && TOLOWER(cp[1]) == 'x')
            cp += 2;
    }
    while (isxdigit(*cp) &&
           (value = isdigit(*cp) ? *cp - '0' : TOLOWER(*cp) - 'a' + 10) < base)
    {
        result = result * base + value;
        cp++;
    }
    if (endp)
        *endp = (char *)cp;
    return result;
}
long strtol(const char *cp, char **endp, unsigned int base)
{
    if (*cp == '-')
        return -strtoul(cp + 1, endp, base);
    return strtoul(cp, endp, base);
}
int atoi(const char *nptr)
{
    return strtol(nptr, (char **)NULL, 10);
}