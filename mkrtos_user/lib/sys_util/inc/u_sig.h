#pragma once
#include <sys/types.h>
enum signal_val
{
    KILL_SIG,
};
typedef int (*sig_call_back)(pid_t pid, umword_t sig_val);

void sig_init(void);
int pm_sig_watch(pid_t pid, int flags);
sig_call_back pm_sig_func_set(sig_call_back sig_func);
int pm_sig_del_watch(pid_t pid, int flags);