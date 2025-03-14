#pragma once

#include "u_types.h"

int sig_kill(obj_handler_t sig_hd, int flags, int resv);
int sig_to_kill(obj_handler_t sig_hd, int flags, int resv);
