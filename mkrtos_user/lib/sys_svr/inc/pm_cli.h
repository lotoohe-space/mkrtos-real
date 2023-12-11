#pragma once

#define PM_KILL_TASK_ALL 0x1

int pm_run_app(const char *path, int flags);
int pm_kill_task(int pid, int flags);
