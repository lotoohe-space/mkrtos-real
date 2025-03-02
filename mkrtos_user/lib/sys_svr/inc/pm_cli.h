#pragma once

#include "u_types.h"
#include <sys/types.h>
#define PM_KILL_TASK_ALL 0x1

int pm_run_app(const char *path, int flags, uint8_t *params, int params_len);
int pm_kill_task(int pid, int flags, int exit_code);
int pm_watch_pid(obj_handler_t sig_hd, pid_t pid, int flags);
int pm_copy_data(pid_t src_pid, pid_t dst_pid, addr_t src_addr, addr_t dst_addr, size_t len);
int pm_del_watch_pid(pid_t pid, int flags);
