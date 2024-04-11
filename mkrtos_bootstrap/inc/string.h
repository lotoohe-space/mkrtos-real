#pragma once

#include "types.h"

void *memccpy(void *dst, const void *src, int c, size_t count);
void memcpy(void *dst, void *src, size_t len);
void *memset(void *dst, char s, size_t count);
int strcmp(const char *s1, const char *s2);
char *strcpy(register char *s1, register const char *s2);
size_t strlen(const char *s);
char *strncpy(char *dest, const char *src, uint32_t n);