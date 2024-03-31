#pragma once

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

typedef unsigned long long uint64_t;
typedef long long int64_t;

typedef unsigned long umword_t;
typedef unsigned short uhmword_t;
typedef long mword_t;

typedef signed long int intptr_t;
typedef unsigned long int uintptr_t;

typedef umword_t size_t;
typedef mword_t ssize_t;
typedef umword_t ptr_t;
typedef char bool_t;
typedef unsigned long obj_addr_t;
typedef umword_t uintptr_t;
typedef umword_t addr_t;
typedef umword_t obj_handler_t;
typedef umword_t time_t;
typedef umword_t pid_t;
#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!(FALSE))
#endif

#define WORD_BYTES (sizeof(void *))
#define WORD_BITS (sizeof(void *) * 8)

#define __ALIGN__(size) __attribute__((aligned(size)))

#define read_reg(addr) (*((volatile umword_t *)(addr)))
#define write_reg(addr, data)                    \
    do                                           \
    {                                            \
        (*((volatile umword_t *)(addr))) = data; \
    } while (0)

#define read_reg32(addr) (*((volatile uint32_t *)(addr)))
#define write_reg32(addr, data)                            \
    do                                                     \
    {                                                      \
        (*((volatile uint32_t *)(addr))) = (uint32_t)data; \
    } while (0)

void jump2kernel(addr_t cpio_start, addr_t cpio_end);
