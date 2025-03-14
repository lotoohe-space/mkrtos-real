#pragma once

#include <types.h>
#include <irq_sender.h>

typedef struct irq_entry
{
    irq_sender_t *irq;
    int inx;
    void (*irq_tigger_func)(struct irq_entry *irq);
} irq_entry_t;

#define IRQ_INVALID_NO ((umword_t)(-1))
// #define CONFIG_IRQ_REG_TAB_SIZE 80
// #define CONFIG_USER_ISR_START_NO 16

bool_t irq_check_usability(int inx);
bool_t irq_alloc(int inx, void *irq,
                 void (*irq_tigger_func)(irq_entry_t *irq));
void irq_free(int inx);
irq_entry_t *irq_get(int inx);
