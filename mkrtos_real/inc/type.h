#ifndef _TYPE_H__
#define _TYPE_H__

#include <stdint.h>

#ifndef NULL
#define NULL ((void*)(0))
#endif

#ifndef TRUE
#define TRUE 1L
#endif

#ifndef FALSE
#define FALSE 0L
#endif

#include <sys/types.h>
typedef uint32_t    di_size_t;
typedef uint32_t    bk_no_t;
//typedef bk_no_t     ino_t;
//typedef uint16_t    mode_t;
//typedef int32_t     off_t;
//typedef uint32_t    size_t;
//typedef uint32_t    dev_t;
typedef uint32_t user_t;
typedef int32_t     sem_t;
typedef uint32_t ptr_t;
typedef unsigned int nfds_t;
typedef uint32_t mword_t;
typedef char bool;

#endif