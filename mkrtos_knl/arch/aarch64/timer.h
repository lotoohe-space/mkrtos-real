#pragma once
#include <irq.h>
void timer_init(int cpu);
void handle_timer_irq(void);
umword_t sys_tick_cnt_get_current(void);
extern void systick_handler(irq_entry_t *irq);
