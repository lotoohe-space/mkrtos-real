#pragma once

#include "types.h"

int memcmp(const void *vl, const void *vr, size_t n);
void *memccpy(void *dst, const void *src, int c, size_t count);
void *memcpy(void *dst, const void *src, size_t len);
void *memset(void *dst, char s, size_t count);
int strcmp(const char *s1, const char *s2);
char *strcpy(register char *s1, register const char *s2);
size_t strlen(const char *s);
char *strncpy(char *dest, const char *src, uint32_t n);
char *strchr(const char *s, int c);
void *memmove(void *dest, const void *src, size_t n);
void *memchr(const void *src, int c, size_t n);
char *strchrnul(const char *s, int c);
void *memrchr(const void *m, int c, size_t n);
char *strrchr(const char *s, int c);
size_t strnlen(const char *s, size_t n);
unsigned long strtoul(const char *cp, char **endp, unsigned int base);
long strtol(const char *cp, char **endp, unsigned int base);
int atoi(const char *nptr);
