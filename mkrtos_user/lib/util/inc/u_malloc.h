#pragma once

struct allocmem
{
    void *ptr;
    size_t size;
    struct allocmem *next;
};

void *u_malloc(size_t size);
void *u_calloc(size_t size, int nmemb);
void *u_realloc(void *old, size_t size);
void u_free(void *ptr);
// void spurge();
