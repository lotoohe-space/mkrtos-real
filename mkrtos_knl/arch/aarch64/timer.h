#pragma once
#include <irq.h>
void timer_init(int cpu);
void handle_timer_irq(void);

extern void systick_handler(irq_entry_t *irq);
