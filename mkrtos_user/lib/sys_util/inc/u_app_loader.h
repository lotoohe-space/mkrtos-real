#pragma once
#include <sys/types.h>
#include "u_env.h"

int app_load(const char *name, uenv_t *cur_env, pid_t *pid, char *argv[], int arg_cn, char *env[], int env_cn);
