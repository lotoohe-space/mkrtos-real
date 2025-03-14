
#pragma once

typedef signed char int8_t;   //!< 8位有符号整数
typedef signed short int16_t; //!< 16位有符号整数
typedef signed int int32_t;   //!< 32位有符号整数

typedef unsigned char uint8_t;
typedef unsigned short uint16_t; //!< 16位无符号整数
typedef unsigned int uint32_t;   //!< 32位无符号整数

typedef unsigned long long uint64_t; //!< 64位无符号整数
typedef long long int64_t;

typedef unsigned long umword_t; //!< 无符号字
typedef unsigned short uhmword_t;
typedef long mword_t;
typedef short uhumword_t;

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
