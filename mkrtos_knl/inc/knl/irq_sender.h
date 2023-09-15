#pragma once

#include "types.h"
#include <kobject.h>
#include <ref.h>
#include <thread.h>
struct irq_sender;
typedef struct irq_sender irq_sender_t;
struct irq_entry;
typedef struct irq_entry irq_entry_t;

typedef struct irq_entry
{
    irq_sender_t *irq;
    void (*irq_tigger_func)(irq_entry_t *irq);
} irq_entry_t;

typedef struct irq_sender
{
    kobject_t kobj;
    umword_t irq_id; //!< 绑定的中断号
    ref_counter_t ref;
    thread_t *wait_thread; //!< 等待中断的线程
    umword_t irq_cn;
} irq_sender_t;

void irq_sender_init(irq_sender_t *irq);
int irq_sender_wait(irq_sender_t *irq, thread_t *th);
void irq_sender_syscall(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f);
