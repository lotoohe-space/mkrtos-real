#pragma once

#include "u_types.h"
#include <sys/types.h>
#define PM_KILL_TASK_ALL 0x1

int pm_run_app(const char *path, int flags);
int pm_kill_task(int pid, int flags);
int pm_watch_pid(obj_handler_t sig_hd, pid_t pid, int flags);
