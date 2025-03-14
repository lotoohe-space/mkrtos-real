#pragma once

#include <u_types.h>
#include <sys/types.h>

int mk_copy_mem_to_task(pid_t src_pid, void *src, pid_t dst_pid, void *dst, size_t len);
