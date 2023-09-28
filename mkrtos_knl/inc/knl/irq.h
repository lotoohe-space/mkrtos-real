#pragma once

#include <types.h>
#include <irq_sender.h>

#define IRQ_INVALID_NO ((umword_t)(-1))
#define IRQ_REG_TAB_SIZE 80
#define USER_ISR_START_NO 16

bool_t irq_check_usability(int inx);
bool_t irq_alloc(int inx, irq_sender_t *irq,
                 void (*irq_tigger_func)(irq_entry_t *irq));
void irq_free(int inx);
irq_entry_t *irq_get(int inx);
