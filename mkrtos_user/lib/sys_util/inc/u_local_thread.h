#pragma once

#include "u_types.h"
#include "u_hd_man.h"

int thread_create(int prio, void *(*fn)(void *arg), umword_t stack_top, obj_handler_t *ret_hd);
