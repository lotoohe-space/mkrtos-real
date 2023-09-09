
#pragma once

#include <stdint.h>
#include "u_arch.h"
#include "u_util.h"
typedef unsigned long umword_t;
typedef long mword_t;
typedef unsigned short uhmword_t;
typedef umword_t ptr_t;
typedef char bool_t;
typedef unsigned long obj_addr_t;
typedef unsigned long obj_handler_t;
typedef umword_t addr_t;
#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!(FALSE))
#endif
