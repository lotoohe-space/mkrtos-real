#pragma once

#include <thread.h>
#include <task.h>
void thread_arch_init(thread_t *th, umword_t flags);
umword_t task_get_currnt_paddr(vaddr_t vaddr);
paddr_t task_get_paddr(task_t *task, vaddr_t vaddr);
