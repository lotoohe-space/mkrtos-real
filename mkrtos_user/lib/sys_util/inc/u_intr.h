#pragma once

#include <u_types.h>
#include <u_irq_sender.h>

int u_intr_bind(int irq_no, u_irq_prio_t prio, int th_prio,
                void *stack, umword_t stack_size, void *msg_buf, void (*thread_func)(void));